/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017 Raffaello D. Di Napoli

This file is part of RAWR.

RAWR is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

RAWR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#pragma once

#include <rawr/alias.hxx>
#include <rawr/bitmanip.hxx>
#include <rawr/chrono.hxx>
#include <rawr/function.hxx>
#include <rawr/hw/timer_counter.hxx>
#include <rawr/misc.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace _pvt {

template <uint8_t Index, char Comparator>
struct timer_mux_base_asm;

#define _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM_IMPL(index, comparator_ascii, vector, prefix) \
   template <> \
   struct timer_mux_base_asm<index, comparator_ascii> { \
      static void emit() { \
         RAWR_ALIAS(RAWR_TOSTRING(vector), prefix "EE8__vectorEv"); \
         RAWR_ALIAS(prefix "EE6delaysE", prefix "EE12delays_bytesE"); \
      } \
   };
#define _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM(index, comparator_ascii, vector) \
   _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM_IMPL(index, comparator_ascii, vector, "_ZN4rawr4_pvt14timer_mux_baseILh" RAWR_TOSTRING(index) "ELc" RAWR_TOSTRING(comparator_ascii))
#ifdef TIMER0_COMPA_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM(0, 65 /*A*/, TIMER0_COMPA_vect)
#endif
#ifdef TIMER0_COMPB_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM(0, 66 /*B*/, TIMER0_COMPB_vect)
#endif
#ifdef TIMER1_COMPA_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM(1, 65 /*A*/, TIMER1_COMPA_vect)
#endif
#ifdef TIMER1_COMPB_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM(1, 66 /*B*/, TIMER1_COMPB_vect)
#endif
#ifdef TIMER2_COMPA_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM(2, 65 /*A*/, TIMER2_COMPA_vect)
#endif
#ifdef TIMER2_COMPB_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM(2, 66 /*B*/, TIMER2_COMPB_vect)
#endif
#undef _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM
#undef _RAWR_SPECIALIZE_TIMER_DELAY_BASE_ASM_IMPL

template <uint8_t Index, char Comparator>
class timer_mux_base {
protected:
   typedef hw::timer_counter<Index> tc;
   typedef typename tc::template comparators<Comparator> tc_comp;
   typedef typename tc::value_type timer_ticks;

public:
   static constexpr chrono::hertz frequency{F_CPU};
   static constexpr chrono::milliseconds max_duration{3000};
   static constexpr timer_ticks max_timer_ticks = static_cast<timer_ticks>(~timer_ticks());

protected:
   struct delay {
      uint16_t remaining_ticks;
      uint16_t initial_ticks;
      function<void ()> callback;

      constexpr bool active() const {
         return callback != nullptr;
      }
   };

public:
   // Given a goal of max tick duration = 500 µs, returns the largest prescaler appropriate for frequency.
   static constexpr uint16_t default_prescaler() {
      return prescaled_period(1024) <= 500000_ns ? 1024 :
             prescaled_period( 256) <= 500000_ns ?  256 :
             prescaled_period(  64) <= 500000_ns ?   64 :
                                                      8;
   }

   /* Given a goal of avoiding overflow of uint16_t for max_duration, returns a “milliscaler” appropriate for
   frequency and the given prescaler. A milliscaler must divide 1000 yielding an integer quotient. */
   static constexpr uint16_t default_milliscaler(uint16_t prescaler) {
      return milliscaled_ticks<uint32_t>(max_duration, prescaler, 125) < 0xffff ?  125 :
             milliscaled_ticks<uint32_t>(max_duration, prescaler, 250) < 0xffff ?  250 :
             milliscaled_ticks<uint32_t>(max_duration, prescaler, 500) < 0xffff ?  500 :
                                                                                  1000;
   }

   template <typename Ret = uint16_t>
   static constexpr Ret milliscaled_ticks(
      chrono::milliseconds delay, uint16_t prescaler, uint16_t milliscaler
   ) {
      return static_cast<Ret>(delay.count()) * static_cast<Ret>(
         int_round_div(int_round_div(frequency, prescaler), milliscaler).count()
      );
   }

protected:
   static constexpr timer_ticks calc_next_ticks(timer_ticks next_ticks, uint16_t remaining_ticks) {
      return static_cast<timer_ticks>(min(
         min(remaining_ticks, static_cast<uint16_t>(next_ticks)),
         static_cast<uint16_t>(~timer_ticks(0)
      )));
   }

private:
   static constexpr chrono::nanoseconds prescaled_period(uint16_t prescaler) {
      return chrono::nanoseconds(int_round_div<uint64_t>(1000000000, int_round_div(frequency, prescaler)));
   }

   /*! Iterates over the delays, invoking the callback for any that expired. It uses the timer to track how
   much time elapsed since it started running, for improved precision.

   It has a weird name to work around g++ name check for interrupt vectors. */
   static __attribute__((signal, used)) void __vector() {
      timer_mux_base_asm<Index, Comparator>::emit();
      timer_ticks curr_ticks = *tc_comp::top, next_ticks = max_timer_ticks;
      bool any_active = false;
      for (auto & delay : delays) {
         if (delay.active()) {
            auto new_remaining_ticks = delay.remaining_ticks - curr_ticks;
            if (static_cast<int16_t>(new_remaining_ticks) <= 0) {
               delay.callback();
               if (delay.initial_ticks == 0) {
                  delay.callback = nullptr;
                  continue;
               }
               new_remaining_ticks = delay.initial_ticks;
            }
            delay.remaining_ticks = new_remaining_ticks;
            next_ticks = calc_next_ticks(next_ticks, new_remaining_ticks);
            any_active = true;
         }
      }
      if (any_active) {
         *tc_comp::top = next_ticks;
         *tc::value = 0;
      } else {
         bitmanip::clear(&TIMSK, tc_comp::interrupt_enable_bit);
      }
   }

protected:
   /* Fixed-size array of delays. This becomes an alias for delays_bytes via asm, so g++ doesn’t generate
   constructor code for it. */
   static delay delays[5];
   /* Storage for delays. */
   static char delays_bytes[sizeof(delays)];
};

template <uint8_t Index, char Comparator>
__attribute__((used)) char timer_mux_base<Index, Comparator>::delays_bytes[sizeof(delays)];

}} //namespace rawr::_pvt

namespace rawr {

template <
   uint8_t Index,
   char Comparator,
   uint16_t Prescaler = _pvt::timer_mux_base<Index, Comparator>::default_prescaler(),
   uint16_t Milliscaler = _pvt::timer_mux_base<Index, Comparator>::default_milliscaler(Prescaler)
>
class timer_mux :
   public _pvt::timer_mux_base<Index, Comparator>,
   public hw::timer_counter_setup<Index, Prescaler> {
private:
   typedef _pvt::timer_mux_base<Index, Comparator> timer_mux_base_;
   using typename timer_mux_base_::delay;
   using typename timer_mux_base_::tc;
   using typename timer_mux_base_::tc_comp;
   using typename timer_mux_base_::timer_ticks;

public:
   static uint8_t once_or_repeat(
      chrono::milliseconds duration, bool recurring, function<void ()> const & callback
   ) {
      delay * ret_delay = nullptr;
      timer_ticks next_ticks = timer_mux_base_::max_timer_ticks;
      for (auto & delay : timer_mux::delays) {
         if (!delay.active()) {
            if (ret_delay) {
               // Already picked a return value.
               continue;
            }
            /* Directly calculating:

               ticks = duration * 1000 / (frequency / Prescaler);

            would overflow uint16_t on duration * 1000. On the other hand,

               ticks = duration / (frequency / Prescaler / 1000);

            would introduce too much error on the frequency divisions for lower frequencies, even with proper
            integer rounding.

            Instead, calculate the number of ticks by using a “milliscaler” factor that ensures duration, up
            to a value of at least max_duration, will not exceed 0xffff, while at the same time avoiding
            excessive denominator values for the frequency division.
            */
            delay.remaining_ticks = int_round_div(
               timer_mux::milliscaled_ticks(duration, Prescaler, Milliscaler),
               1000 / Milliscaler /*no remainder*/
            );
            delay.initial_ticks = recurring ? delay.remaining_ticks : 0;
            delay.callback = move(callback);
            ret_delay = &delay;
         }
         next_ticks = timer_mux::calc_next_ticks(next_ticks, delay.remaining_ticks);
      }
      if (!ret_delay) {
         abort();
      }

      *tc_comp::top = next_ticks;
      *tc::value = 0;
      // Enable the Output Compare Interrupt.
      bitmanip::set(&TIMSK, tc_comp::interrupt_enable_bit);

      return static_cast<uint8_t>(ret_delay - &timer_mux::delays[0]);
   }

   static uint8_t once(chrono::milliseconds duration, function<void ()> const & callback) {
      return once_or_repeat(duration, false, callback);
   }

   static uint8_t repeat(chrono::milliseconds duration, function<void ()> const & callback) {
      return once_or_repeat(duration, true, callback);
   }
};

} //namespace rawr
