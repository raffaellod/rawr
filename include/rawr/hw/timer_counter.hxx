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

#include <avr/interrupt.h>
#include <avr/io.h>
#include <rawr/hw/consistent_names.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

template <uint8_t Index>
struct timer_counter;

#ifdef TCNT0
template <>
struct timer_counter<0> {
   static constexpr auto control_register_a = &TCCR0A;
   static constexpr auto control_register_b = &TCCR0B;
   typedef uint8_t value_type;
   static constexpr auto value = &TCNT0;
};
#endif

#ifdef TCNT1
template <>
struct timer_counter<1> {
   static constexpr auto control_register_a = &TCCR1A;
   static constexpr auto control_register_b = &TCCR1B;
   typedef uint16_t value_type;
   static constexpr auto value = &TCNT1;
};
#endif

#ifdef TCNT2
template <>
struct timer_counter<2> {
   static constexpr auto control_register_a = &TCCR2A;
   static constexpr auto control_register_b = &TCCR2B;
   typedef uint8_t value_type;
   static constexpr auto value = &TCNT2;
};
#endif

}} //namespace rawr::hw

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

template <uint8_t Index, char Comparator>
struct timer_counter_comparator;

#define _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(index, comparator, comp_unquoted) \
   template <> \
   struct timer_counter_comparator<index, comparator> { \
      static constexpr uint8_t interrupt_enable_bit = RAWR_CPP_CAT3(OCIE, index, comp_unquoted); \
      static constexpr auto top = &RAWR_CPP_CAT3(OCR, index, comp_unquoted); \
   };
#ifdef OCR0A
   _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(0, 'A', A)
#endif
#ifdef OCR0B
   _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(0, 'B', B)
#endif
#ifdef OCR1A
   _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(1, 'A', A)
#endif
#ifdef OCR1B
   _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(1, 'B', B)
#endif
#ifdef OCR2A
   _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(2, 'A', A)
#endif
#ifdef OCR2B
   _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(2, 'B', B)
#endif
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR

}} //namespace rawr::hw

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

template <uint8_t Index, uint16_t Divider>
struct timer_counter_prescaler;

#define _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(index, factor, bitmask) \
   template <> \
   struct timer_counter_prescaler<index, factor> { \
      static constexpr uint8_t control_register_bitmask = bitmask; \
   };
#ifdef TCNT0
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,    1,        0  |        0  | _BV(CS00))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,    8,        0  | _BV(CS01) |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,   64,        0  | _BV(CS01) | _BV(CS00))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,  256, _BV(CS02) |        0  |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0, 1024, _BV(CS02) |        0  | _BV(CS00))
#endif
#ifdef TCNT1
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,    1,        0  |        0  | _BV(CS10))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,    8,        0  | _BV(CS11) |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,   64,        0  | _BV(CS11) | _BV(CS10))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,  256, _BV(CS12) |        0  |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1, 1024, _BV(CS12) |        0  | _BV(CS10))
#endif
#ifdef TCNT2
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,    1,        0  |        0  | _BV(CS20))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,    8,        0  | _BV(CS21) |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,   32,        0  | _BV(CS21) | _BV(CS20))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,   64, _BV(CS22) |        0  |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,  128, _BV(CS22) |        0  | _BV(CS20))
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,  256, _BV(CS22) | _BV(CS21) |        0 )
   _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2, 1024, _BV(CS22) | _BV(CS21) | _BV(CS20))
#endif
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER

}} //namespace rawr::hw
