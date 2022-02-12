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

#include <rawr/hw/io.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

//! Defines control registers for each timer/counter available on the selected MCU.
template <int Index>
struct timer_counter_control_registers_raw {
   // Check for just any value known to be false.
   static_assert(Index < 0,
      "the selected MCU does not seem to have this timer, or unsupported TCCRnx config for it"
   );
};

// Specializations need to be done via macro due to register names.
#define _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_RAW_A(index) \
   template <> \
   struct timer_counter_control_registers_raw<index> { \
      static constexpr unsigned count{1}; \
      \
      static constexpr decltype(RAWR_CPP_CAT3(TCCR, index, A)) a{}; \
   };
#define _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_RAW_AB(index) \
   template <> \
   struct timer_counter_control_registers_raw<index> { \
      static constexpr unsigned count{2}; \
      \
      static constexpr decltype(RAWR_CPP_CAT3(TCCR, index, A)) a{}; \
      static constexpr decltype(RAWR_CPP_CAT3(TCCR, index, B)) b{}; \
   };
#define _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_RAW_ABC(index) \
   template <> \
   struct timer_counter_control_registers_raw<index> { \
      static constexpr unsigned count{3}; \
      \
      static constexpr decltype(RAWR_CPP_CAT3(TCCR, index, A)) a{}; \
      static constexpr decltype(RAWR_CPP_CAT3(TCCR, index, B)) b{}; \
      static constexpr decltype(RAWR_CPP_CAT3(TCCR, index, C)) c{}; \
   };

/*! TCCRnx layout.

Not simply a TCCR count, because features may differ from one 2-TCCR timer/counter to another 2-TCCR t/c.
Instead we just call each feature/bits config a “layout”, so when a novel one needs to be added, we can add a
new, incompatible TCC registers/bits configuration. */
template <int Index>
struct timer_counter_control_registers_layout {
   // Check for just any value known to be false.
   static_assert(Index < 0,
      "the selected MCU does not seem to have this timer, or unsupported TCCRnx config for it"
   );
};

#define _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_1(index) \
   template <> \
   struct timer_counter_control_registers_layout<index> { \
      static constexpr int layout{1}; \
   }; \
   _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_RAW_A(index)
#define _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_2(index) \
   template <> \
   struct timer_counter_control_registers_layout<index> { \
      static constexpr int layout{2}; \
   }; \
   _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_RAW_AB(index)
#define _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_3(index) \
   template <> \
   struct timer_counter_control_registers_layout<index> { \
      static constexpr int layout{3}; \
   }; \
   _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_RAW_ABC(index)

/*! Abstracts access to bits in the TCCRnx registers, so that code may be switched from using one timer to
another different one, without any (ideally) changes. */
template <int Index, uint8_t Mask_cs, int = timer_counter_control_registers_layout<Index>::layout>
struct timer_counter_control_registers;

/* TCCR layout 1: one Output Compare Unit, two WGM bits:
   TCCRnA  FOCnA WGMn[0] COMnA[1:0] WGMn[1] CSn[2:0] */
template <int Index, uint8_t Mask_cs>
struct timer_counter_control_registers<Index, Mask_cs, 1> : timer_counter_control_registers_raw<Index> {
   static void set_cs(uint8_t cs) {
      timer_counter_control_registers_raw<Index>::a &= static_cast<uint8_t>(~Mask_cs);
      timer_counter_control_registers_raw<Index>::a |= cs;
   }
};

/* TCCR layout 2: two Output Compare Units, three WGM bits:
   TCCRnA  COMnA[1:0] COMnB[1:0] - - WGMn[1:0]
   TCCRnB  FOCnA FOCnB - - WGMn[2] CSn[2:0] */
template <int Index, uint8_t Mask_cs>
struct timer_counter_control_registers<Index, Mask_cs, 2> : timer_counter_control_registers_raw<Index> {
   static void set_cs(uint8_t cs) {
      timer_counter_control_registers_raw<Index>::b &= static_cast<uint8_t>(~Mask_cs);
      timer_counter_control_registers_raw<Index>::b |= cs;
   }
};

/* TCCR layout 3: two Output Compare Units + one Input Capture Unit, four WGM bits:
   TCCRnA  COMnA[1:0] COMnB[1:0] - - WGMn[1:0]
   TCCRnB  ICNCn ICESn - WGMn[3:2] CSn[2:0]
   TCCRnC  FOCnA FOCnB - - - - - - */
template <int Index, uint8_t Mask_cs>
struct timer_counter_control_registers<Index, Mask_cs, 3> : timer_counter_control_registers_raw<Index> {
   static void set_cs(uint8_t cs) {
      timer_counter_control_registers_raw<Index>::b &= static_cast<uint8_t>(~Mask_cs);
      timer_counter_control_registers_raw<Index>::b |= cs;
   }
};

//! Timer/counter abstraction; provides a uniform interface for all timer/counter units on AVR MCUs.
template <int Index>
struct timer_counter {
   // Check for just any value known to be false.
   static_assert(Index < 0, "the selected MCU does not seem to have this timer");
};

#define _RAWR_SPECIALIZE_TIMER_COUNTER(index) \
   template <> \
   struct timer_counter<index> { \
      static constexpr decltype(RAWR_CPP_CAT2(TCNT, index)) value{}; \
      using value_type = decltype(value)::type; \
      static constexpr timer_counter_control_registers< \
         index, \
         _BV(RAWR_CPP_CAT3(CS, index, 2)) | _BV(RAWR_CPP_CAT3(CS, index, 1)) | _BV(RAWR_CPP_CAT3(CS, index, 0)) \
      > control_registers{}; \
      \
      template <char Comparator> \
      struct comparators { \
         /* Check for just any value known to be false. */ \
         static_assert(!Comparator, "the selected MCU does not seem to have this timer/comparator combination"); \
      }; \
      \
      template <unsigned Prescaler> \
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
#define _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(index, prescaler, cr_bits) \
   template <> \
   struct timer_counter<index>::prescalers<prescaler> { \
      static constexpr uint8_t control_register_bits = cr_bits; \
   };
#ifdef TCNT0
   #ifdef OCR0A
      #ifdef OCR0B
         #ifdef ICR0
            _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_3(0)
         #else
            _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_2(0)
         #endif
      #else
         _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_1(0)
      #endif
   #else
      #error "unknown TCCR layout for timer/counter 0"
   #endif
   _RAWR_SPECIALIZE_TIMER_COUNTER(0)
   #ifdef OCR0A
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(0, 'A', A)
   #endif
   #ifdef OCR0B
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(0, 'B', B)
   #endif
   #ifdef T0_PIN
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,    1,        0  |        0  | _BV(CS00))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,    8,        0  | _BV(CS01) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,   64,        0  | _BV(CS01) | _BV(CS00))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,  256, _BV(CS02) |        0  |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0, 1024, _BV(CS02) |        0  | _BV(CS00))
      // 110 and 111 use T0 as clock source.
   #else
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,    1,        0  |        0  | _BV(CS00))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,    8,        0  | _BV(CS01) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,   32,        0  | _BV(CS01) | _BV(CS00))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,   64, _BV(CS02) |        0  |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,  128, _BV(CS02) |        0  | _BV(CS00))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0,  256, _BV(CS02) | _BV(CS01) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(0, 1024, _BV(CS02) | _BV(CS01) | _BV(CS00))
   #endif
#endif
#ifdef TCNT1
   #ifdef OCR1A
      #ifdef OCR1B
         #ifdef ICR1
            _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_3(1)
         #else
            _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_2(1)
         #endif
      #else
         _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_1(1)
      #endif
   #else
      #error "unknown TCCR layout for timer/counter 1"
   #endif
   _RAWR_SPECIALIZE_TIMER_COUNTER(1)
   #ifdef OCR1A
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(1, 'A', A)
   #endif
   #ifdef OCR1B
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(1, 'B', B)
   #endif
   #ifdef T1_PIN
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,    1,        0  |        0  | _BV(CS10))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,    8,        0  | _BV(CS11) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,   64,        0  | _BV(CS11) | _BV(CS10))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,  256, _BV(CS12) |        0  |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1, 1024, _BV(CS12) |        0  | _BV(CS10))
      // 110 and 111 use T1 as clock source.
   #else
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,    1,        0  |        0  | _BV(CS10))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,    8,        0  | _BV(CS11) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,   32,        0  | _BV(CS11) | _BV(CS10))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,   64, _BV(CS12) |        0  |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,  128, _BV(CS12) |        0  | _BV(CS10))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1,  256, _BV(CS12) | _BV(CS11) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(1, 1024, _BV(CS12) | _BV(CS11) | _BV(CS10))
   #endif
#endif
#ifdef TCNT2
   #ifdef OCR2A
      #ifdef OCR2B
         #ifdef ICR2
            _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_3(2)
         #else
            _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_2(2)
         #endif
      #else
         _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_1(2)
      #endif
   #else
      #error "unknown TCCR layout for timer/counter 2"
   #endif
   _RAWR_SPECIALIZE_TIMER_COUNTER(2)
   #ifdef OCR2A
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(2, 'A', A)
   #endif
   #ifdef OCR2B
      _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR(2, 'B', B)
   #endif
   #ifdef T2_PIN
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,    1,        0  |        0  | _BV(CS20))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,    8,        0  | _BV(CS21) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,   64,        0  | _BV(CS21) | _BV(CS20))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,  256, _BV(CS22) |        0  |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2, 1024, _BV(CS22) |        0  | _BV(CS20))
      // 110 and 111 use T2 as clock source.
   #else
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,    1,        0  |        0  | _BV(CS20))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,    8,        0  | _BV(CS21) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,   32,        0  | _BV(CS21) | _BV(CS20))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,   64, _BV(CS22) |        0  |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,  128, _BV(CS22) |        0  | _BV(CS20))
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2,  256, _BV(CS22) | _BV(CS21) |        0 )
      _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER(2, 1024, _BV(CS22) | _BV(CS21) | _BV(CS20))
   #endif
#endif

#undef _RAWR_SPECIALIZE_TIMER_COUNTER
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_COMPARATOR
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_1
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_2
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_LAYOUT_3
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_A
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_AB
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_CONTROL_REGISTERS_ABC
#undef _RAWR_SPECIALIZE_TIMER_COUNTER_PRESCALER

}} //namespace rawr::hw
