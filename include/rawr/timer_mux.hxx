/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2022 Raffaello D. Di Napoli

This file is part of RAWR.

RAWR is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

RAWR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#pragma once

#include <rawr/abort.hxx>
#include <rawr/alias.hxx>
#include <rawr/chrono.hxx>
#include <rawr/function.hxx>
#include <rawr/hw/timer_counter.hxx>
#include <rawr/misc.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace _pvt {

template <int Index, char Comparator>
struct timer_mux_asm;

#define _RAWR_SPECIALIZE_TIMER_DELAY_ASM_IMPL(index, comparator_ascii, vector, prefix) \
   template <> \
   struct timer_mux_asm<index, comparator_ascii> { \
      static void emit() { \
         RAWR_ALIAS(RAWR_TOSTRING(vector), prefix "EE8__vectorEv"); \
      } \
   };
#define _RAWR_SPECIALIZE_TIMER_DELAY_ASM(index, comparator_ascii, vector) \
   _RAWR_SPECIALIZE_TIMER_DELAY_ASM_IMPL(index, comparator_ascii, vector, "_ZN4rawr9timer_muxILi" RAWR_TOSTRING(index))
#ifdef TIMER0_COMPA_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_ASM(0, 65 /*A*/, TIMER0_COMPA_vect)
#endif
#ifdef TIMER0_COMPB_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_ASM(0, 66 /*B*/, TIMER0_COMPB_vect)
#endif
#ifdef TIMER1_COMPA_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_ASM(1, 65 /*A*/, TIMER1_COMPA_vect)
#endif
#ifdef TIMER1_COMPB_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_ASM(1, 66 /*B*/, TIMER1_COMPB_vect)
#endif
#ifdef TIMER2_COMPA_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_ASM(2, 65 /*A*/, TIMER2_COMPA_vect)
#endif
#ifdef TIMER2_COMPB_vect
   _RAWR_SPECIALIZE_TIMER_DELAY_ASM(2, 66 /*B*/, TIMER2_COMPB_vect)
#endif
#undef _RAWR_SPECIALIZE_TIMER_DELAY_ASM
#undef _RAWR_SPECIALIZE_TIMER_DELAY_ASM_IMPL

//! Calculates the period resulting from applying the given prescaling factor to F_CPU.
inline constexpr chrono::nanoseconds prescaled_period(uint16_t prescaler) {
   return chrono::nanoseconds(int_round_div<uint64_t>(1000000000, int_round_div<uint64_t>(F_CPU, prescaler)));
}

//! Given a goal of max tick duration = 500 µs, returns the largest prescaler appropriate for frequency.
inline constexpr uint16_t default_timer_mux_prescaler() {
   return prescaled_period(1024) <= 500000_ns ? 1024 :
          prescaled_period( 256) <= 500000_ns ?  256 :
          prescaled_period(  64) <= 500000_ns ?   64 :
                                                   8;
}

}} //namespace rawr::_pvt

namespace rawr {

/*! Millisecond-precision timer multiplexer. Allows to create many delays or virtual timers while consuming a
single hardware timer.

Once created, a delay can be canceled by invoking cancel() on the returned object. Delays created with the
repeat() method are automatically re-scheduled; those created with once() are not.

The range for delays is 1 millisecond up to min_max_duration (currently 3 seconds); this range is chosen to
avoid 32-bit math operations while at the same time minimizing the number of interrupts generated (i.e.
wake-ups from sleep). */
template <int Index>
class timer_mux {
public:
   static constexpr chrono::hertz frequency{F_CPU};
   static constexpr chrono::milliseconds min_max_duration{3000_ms};

public:
   struct delay_control {
   private:
      friend class timer_mux;

   public:
      constexpr delay_control() :
         index{-1} {
      }

      explicit constexpr operator bool() const {
         return index >= 0;
      }

      void cancel() {
         // Don’t bother rescheduling the timer; that will happen when the interrupt occurs.
         timer_mux::delays[index].callback = nullptr;
      }

   private:
      explicit delay_control(uint8_t index_) :
         index{static_cast<int8_t>(index_)} {
      }

   private:
      int8_t index;
   };

private:
   static constexpr char comparator_name{'A'};
   typedef hw::timer_counter<Index> timer_counter_t;
   typedef typename timer_counter_t::template comparators<comparator_name> tc_comp;
   typedef typename timer_counter_t::value_type timer_ticks;

   static constexpr timer_ticks max_timer_ticks{static_cast<timer_ticks>(~timer_ticks{})};
   static constexpr uint16_t prescaler{_pvt::default_timer_mux_prescaler()};
   typedef typename timer_counter_t::template prescalers<prescaler> tc_prescaler;

   /* Given a goal of avoiding overflow of uint16_t for min_max_duration, returns a “milliscaler” appropriate
   for frequency and the given prescaler. A milliscaler must divide 1000 yielding an integer quotient. */
   static constexpr uint16_t default_milliscaler() {
      return milliscaled_ticks<uint32_t>(min_max_duration, prescaler, 125) < 0xffff ?  125 :
             milliscaled_ticks<uint32_t>(min_max_duration, prescaler, 250) < 0xffff ?  250 :
             milliscaled_ticks<uint32_t>(min_max_duration, prescaler, 500) < 0xffff ?  500 :
                                                                                      1000;
   }

   static constexpr uint16_t milliscaler = default_milliscaler();

   struct delay_t {
      uint16_t remaining_ticks;
      uint16_t initial_ticks;
      function<void ()> callback;

      constexpr bool active() const {
         return callback != nullptr;
      }
   };

public:
   timer_mux() {
      static_this = this;
      timer.control_registers.set_cs(tc_prescaler::control_register_bits);
   }

   delay_control once_or_repeat(
      chrono::milliseconds duration, bool recurring, function<void ()> const & callback
   ) {
      delay_t * ret_delay{nullptr};
      timer_ticks next_ticks{max_timer_ticks};
      /* Disable the timer interrupt to make sure delays doesn’t change in the middle of iterating over it,
      then get the current counter value to account for any spent ticks, since we’ll be resetting it to 0 at
      the end of this function. */
      TIMSK.clear_bit(tc_comp::interrupt_enable_bit);
      timer_ticks curr_ticks{timer.value};
      for (auto & delay : delays) {
         if (delay.active()) {
            // Won’t be negative, or the interrupt would have been triggered.
            delay.remaining_ticks -= curr_ticks;
         } else {
            if (ret_delay) {
               // Already picked a return value.
               continue;
            }
            /* Directly calculating:

               ticks = duration * 1000 / (frequency / prescaler);

            would overflow uint16_t on duration * 1000. On the other hand,

               ticks = duration / (frequency / prescaler / 1000);

            would introduce too much error on the frequency divisions for lower frequencies, even with proper
            integer rounding.

            Instead, calculate the number of ticks by using a “milliscaler” factor that ensures duration, up
            to a value of at least min_max_duration, will not exceed 0xffff, while at the same time avoiding
            excessive denominator values for the frequency division.
            */
            delay.remaining_ticks = int_round_div(
               milliscaled_ticks(duration, prescaler, milliscaler),
               1000 / milliscaler /*no remainder*/
            );
            delay.initial_ticks = recurring ? delay.remaining_ticks : 0;
            delay.callback = move(callback);
            ret_delay = &delay;
         }
         next_ticks = calc_next_ticks(next_ticks, delay.remaining_ticks);
      }
      if (!ret_delay) {
         abort();
      }

      // Reset the timer and (re-)enable the interrupt.
      tc_comp::top = next_ticks;
      timer.value = 0;
      TIMSK.set_bit(tc_comp::interrupt_enable_bit);

      return delay_control{static_cast<uint8_t>(ret_delay - &delays[0])};
   }

   /*! Schedules a delayed call to the specified callback. The call can be prevented by invoking cancel() on
   the returned object. */
   delay_control once(chrono::milliseconds delay, function<void ()> const & callback) {
      return once_or_repeat(delay, false, callback);
   }

   /*! Schedules a virtual timer to repeatedly call the specified callback. Invoking cancel() on the returned
   object ends the calls. */
   delay_control repeat(chrono::milliseconds period, function<void ()> const & callback) {
      return once_or_repeat(period, true, callback);
   }

private:
   static constexpr timer_ticks calc_next_ticks(timer_ticks next_ticks, uint16_t remaining_ticks) {
      return static_cast<timer_ticks>(min(
         min(remaining_ticks, static_cast<uint16_t>(next_ticks)),
         static_cast<uint16_t>(~timer_ticks{0})
      ));
   }

   /*! Iterates over the delays, invoking the callback for any that expired. It uses the timer to track how
   much time elapsed since it started running, for improved precision. */
   void interrupt() {
      // Must use tc_comp::top instead of timer.value because by now value has already been reset to 0.
      timer_ticks curr_ticks = tc_comp::top, next_ticks = max_timer_ticks;
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
         // Reset the timer, and keep the interrupt enabled.
         tc_comp::top = next_ticks;
         timer.value = 0;
      } else {
         TIMSK.clear_bit(tc_comp::interrupt_enable_bit);
      }
   }

   template <typename Ret = uint16_t>
   static constexpr Ret milliscaled_ticks(
      chrono::milliseconds delay, uint16_t prescaler, uint16_t milliscaler
   ) {
      return static_cast<Ret>(delay.count()) * static_cast<Ret>(
         int_round_div(int_round_div(frequency, prescaler), milliscaler).count()
      );
   }

   //! Interrupt vector. It has a weird name to work around g++’s name check for interrupt vectors.
   static __attribute__((signal, used)) void __vector() {
      _pvt::timer_mux_asm<Index, comparator_name>::emit();
      static_this->interrupt();
   }

private:
   timer_counter_t timer;
   // Fixed-size array of delays.
   delay_t delays[5];
   // Used to find *this from __vector().
   static timer_mux * static_this;
};

template <int Index>
__attribute__((used)) timer_mux<Index> * timer_mux<Index>::static_this;

} //namespace rawr
