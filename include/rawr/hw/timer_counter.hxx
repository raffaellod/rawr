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

#include <rawr/hw/io.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

template <uint8_t Index>
struct timer_counter {
   // Check for just any value known to be false.
   static_assert(Index != 0xff, "the selected MCU does not seem to have this timer");
};

#define _RAWR_SPECIALIZE_TIMER_COUNTER(index, uint_t) \
   template <> \
   struct timer_counter<index> { \
      static constexpr decltype(RAWR_CPP_CAT3(TCCR, index, A)) control_register_a{}; \
      static constexpr decltype(RAWR_CPP_CAT3(TCCR, index, B)) control_register_b{}; \
      typedef uint_t value_type; \
      static constexpr decltype(RAWR_CPP_CAT2(TCNT, index)) value{}; \
      \
      template <char Comparator> \
      struct comparators { \
         /* Check for just any value known to be false. */ \
         static_assert(!Comparator, "the selected MCU does not seem to have this timer/comparator combination"); \
      }; \
      \
      template <uint16_t Prescaler> \
      struct prescalers { \
         /* Check for just any value known to be false. */ \
         static_assert(!Prescaler, "the selected MCU does not seem to have this timer/prescaler combination"); \
      }; \
   };
#define _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(index, comparator, comp_unquoted) \
   template <> \
   struct timer_counter<index>::comparators<comparator> { \
      static constexpr uint8_t interrupt_enable_bit = RAWR_CPP_CAT3(OCIE, index, comp_unquoted); \
      static constexpr decltype(RAWR_CPP_CAT3(OCR, index, comp_unquoted)) top{}; \
   };
#define _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(index, prescaler, bits) \
   template <> \
   struct timer_counter<index>::prescalers<prescaler> { \
      static constexpr uint8_t control_register_bits = bits; \
   };
#ifdef TCNT0
   _RAWR_SPECIALIZE_TIMER_COUNTER(0, uint8_t)
   #ifdef OCR0A
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(0, 'A', A)
   #endif
   #ifdef OCR0B
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(0, 'B', B)
   #endif
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,    1,        0  |        0  | _BV(CS00))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,    8,        0  | _BV(CS01) |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,   64,        0  | _BV(CS01) | _BV(CS00))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,  256, _BV(CS02) |        0  |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0, 1024, _BV(CS02) |        0  | _BV(CS00))
#endif
#ifdef TCNT1
   _RAWR_SPECIALIZE_TIMER_COUNTER(1, uint16_t)
   #ifdef OCR1A
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(1, 'A', A)
   #endif
   #ifdef OCR1B
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(1, 'B', B)
   #endif
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,    1,        0  |        0  | _BV(CS10))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,    8,        0  | _BV(CS11) |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,   64,        0  | _BV(CS11) | _BV(CS10))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,  256, _BV(CS12) |        0  |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1, 1024, _BV(CS12) |        0  | _BV(CS10))
#endif
#ifdef TCNT2
   _RAWR_SPECIALIZE_TIMER_COUNTER(2, uint8_t)
   #ifdef OCR2A
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(2, 'A', A)
   #endif
   #ifdef OCR2B
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(2, 'B', B)
   #endif
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,    1,        0  |        0  | _BV(CS20))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,    8,        0  | _BV(CS21) |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,   32,        0  | _BV(CS21) | _BV(CS20))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,   64, _BV(CS22) |        0  |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,  128, _BV(CS22) |        0  | _BV(CS20))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,  256, _BV(CS22) | _BV(CS21) |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2, 1024, _BV(CS22) | _BV(CS21) | _BV(CS20))
#endif
#undef _RAWR_SPECIALIZE_TIMER_COUNTER
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER

}} //namespace rawr::hw
