/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2020, 2022 Raffaello D. Di Napoli

This file is part of RAWR.

RAWR is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

RAWR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#pragma once

#include <rawr/hw/io.hxx>
#include <rawr/misc.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw { namespace _pvt {

/*! This struct, combined with io_port_pcint_members, allows to map from I/O port names (“A”, “B”, …) to PCINT
IRQ indices, and inject PCINT-related members into rawr::hw::io_port. */
template <int Irq>
struct irq_to_pcint_index;

#define _RAWR_SPECIALIZE_IRQ_TO_PCINT_INDEX(irq, index) \
   template <> \
   struct irq_to_pcint_index<irq> { \
      static constexpr int value = index; \
   };
#ifdef PCINT0_vect_num
   _RAWR_SPECIALIZE_IRQ_TO_PCINT_INDEX(PCINT0_vect_num, 0)
#endif
#ifdef PCINT1_vect_num
   _RAWR_SPECIALIZE_IRQ_TO_PCINT_INDEX(PCINT1_vect_num, 1)
#endif
#ifdef PCINT2_vect_num
   _RAWR_SPECIALIZE_IRQ_TO_PCINT_INDEX(PCINT2_vect_num, 2)
#endif
#ifdef PCINT3_vect_num
   _RAWR_SPECIALIZE_IRQ_TO_PCINT_INDEX(PCINT3_vect_num, 3)
#endif
#undef _RAWR_SPECIALIZE_IRQ_TO_PCINT_INDEX

//! Provides access to PCINT registers and bits by PCINT IRQ index.
template <int Index>
struct io_port_pcint;

#define _RAWR_SPECIALIZE_IO_PORT_PCINT(index, pcmsk, pcie, pcif) \
   template <> \
   struct io_port_pcint<index> { \
      static constexpr uint8_t pcint_enable_bit = pcie; \
      static constexpr uint8_t pcint_flag_bit = pcif; \
      static constexpr decltype(pcmsk) pcint_mask{}; \
   };
#ifdef PCIE0
   _RAWR_SPECIALIZE_IO_PORT_PCINT(0, PCMSK0, PCIE0, PCIF0)
#endif
#ifdef PCIE1
   _RAWR_SPECIALIZE_IO_PORT_PCINT(1, PCMSK1, PCIE1, PCIF1)
#endif
#ifdef PCIE2
   _RAWR_SPECIALIZE_IO_PORT_PCINT(2, PCMSK2, PCIE2, PCIF2)
#endif
#ifdef PCIE3
   _RAWR_SPECIALIZE_IO_PORT_PCINT(3, PCMSK3, PCIE3, PCIF3)
#endif
#undef _RAWR_SPECIALIZE_IO_PORT_PCINT

//! PCINT-related members injected into rawr::hw::io_port.
template <char Port>
struct io_port_pcint_members {
   //! Indicates whether PCINT for the I/O port is supported.
   static constexpr bool has_pcint{false};
};

#define _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS(port, irq) \
   template <> \
   struct io_port_pcint_members<port> : io_port_pcint<irq_to_pcint_index<irq>::value> { \
      static constexpr bool has_pcint{true}; \
   };
#ifdef PCINT_A_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('A', PCINT_A_vect_num)
#endif
#ifdef PCINT_B_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('B', PCINT_B_vect_num)
#endif
#ifdef PCINT_C_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('C', PCINT_C_vect_num)
#endif
#ifdef PCINT_D_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('D', PCINT_D_vect_num)
#endif
#ifdef PCINT_E_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('E', PCINT_E_vect_num)
#endif
#ifdef PCINT_F_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('F', PCINT_F_vect_num)
#endif
#ifdef PCINT_G_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('G', PCINT_G_vect_num)
#endif
#ifdef PCINT_H_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('H', PCINT_H_vect_num)
#endif
#ifdef PCINT_J_vect_num
   _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS('J', PCINT_J_vect_num)
#endif
#undef _RAWR_SPECIALIZE_IO_PORT_PCINT_MEMBERS

}}} //namespace rawr::hw::_pvt

namespace rawr { namespace hw {

//! Provides access to I/O port registers and bits by port name (“A”, “B”, …).
template <char Port>
struct io_port {
   // Check for just any value known to be false.
   static_assert(!Port, "the selected MCU does not seem to have this port");
};

//! Identifies a pin of an I/O port as a type; used as template argument for other classes.
template <char Port, int Pin>
struct io_port_pin {
   // Check for just any value known to be false.
   static_assert(!Port, "the selected MCU does not seem to have this port");
};

#define _RAWR_SPECIALIZE_IO_PORT(port_quoted, port_unquoted, defined_pins) \
   template <> \
   struct io_port<port_quoted> : _pvt::io_port_pcint_members<port_quoted> { \
      static constexpr char name = port_quoted; \
      static constexpr decltype(RAWR_CPP_CAT2(DDR, port_unquoted)) data_direction{}; \
      static constexpr decltype(RAWR_CPP_CAT2(PORT, port_unquoted)) data{}; \
      static constexpr decltype(RAWR_CPP_CAT2(PIN, port_unquoted)) pins{}; \
      static constexpr unsigned bit_size{defined_pins}; \
   }; \
   \
   template <int Pin> \
   struct io_port_pin<port_quoted, Pin> { \
      static_assert(Pin <= defined_pins, "the selected MCU does not seem to have this port pin"); \
      typedef io_port<port_quoted> port; \
      static constexpr auto pin = Pin; \
   };
#ifdef PORTA
   #if defined(PORTA7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTA6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTA5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTA4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTA3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTA2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTA1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTA0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTA is defined, but no PORTAn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('A', A, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#ifdef PORTB
   #if defined(PORTB7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTB6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTB5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTB4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTB3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTB2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTB1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTB0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTB is defined, but no PORTBn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('B', B, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#ifdef PORTC
   #if defined(PORTC7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTC6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTC5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTC4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTC3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTC2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTC1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTC0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTC is defined, but no PORTCn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('C', C, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#ifdef PORTD
   #if defined(PORTD7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTD6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTD5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTD4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTD3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTD2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTD1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTD0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTD is defined, but no PORTDn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('D', D, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#ifdef PORTE
   #if defined(PORTE7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTE6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTE5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTE4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTE3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTE2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTE1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTE0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTE is defined, but no PORTEn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('E', E, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#ifdef PORTF
   #if defined(PORTF7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTF6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTF5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTF4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTF3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTF2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTF1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTF0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTF is defined, but no PORTFn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('F', F, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#ifdef PORTG
   #if defined(PORTG7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTG6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTG5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTG4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTG3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTG2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTG1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTG0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTG is defined, but no PORTGn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('G', G, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#ifdef PORTH
   #if defined(PORTH7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTH6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTH5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTH4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTH3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTH2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTH1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTH0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTH is defined, but no PORTHn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('H', H, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#ifdef PORTJ
   #if defined(PORTJ7)
      #define _RAWR_DEFINED_IO_PORT_PINS 8
   #elif defined(PORTJ6)
      #define _RAWR_DEFINED_IO_PORT_PINS 7
   #elif defined(PORTJ5)
      #define _RAWR_DEFINED_IO_PORT_PINS 6
   #elif defined(PORTJ4)
      #define _RAWR_DEFINED_IO_PORT_PINS 5
   #elif defined(PORTJ3)
      #define _RAWR_DEFINED_IO_PORT_PINS 4
   #elif defined(PORTJ2)
      #define _RAWR_DEFINED_IO_PORT_PINS 3
   #elif defined(PORTJ1)
      #define _RAWR_DEFINED_IO_PORT_PINS 2
   #elif defined(PORTJ0)
      #define _RAWR_DEFINED_IO_PORT_PINS 1
   #else
      #error "PORTJ is defined, but no PORTJn constants are?!"
   #endif
   _RAWR_SPECIALIZE_IO_PORT('J', J, _RAWR_DEFINED_IO_PORT_PINS)
   #undef _RAWR_DEFINED_IO_PORT_PINS
#endif
#undef _RAWR_SPECIALIZE_IO_PORT

}} //namespace rawr::hw
