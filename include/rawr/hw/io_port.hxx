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

#include <avr/io.h>
#include <rawr/bitmanip.hxx>
#include <rawr/hw/consistent_names.hxx>
#include <rawr/misc.hxx>
#include <rawr/static_constructor.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw { namespace _pvt {

/*! This struct, combined with port_to_pcint_irq, allows to map from I/O port names (“A”, “B”, …) to PCINT IRQ
indices. */
template <uint8_t Irq>
struct irq_to_pcint_index;

#define _RAWR_SPECIALIZE_IRQ_TO_PCINT_INDEX(irq, index) \
   template <> \
   struct irq_to_pcint_index<irq> { \
      static constexpr uint8_t value = index; \
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

/*! This struct, combined with irq_to_pcint_index, allows to map from I/O port names (“A”, “B”, …) to PCINT
IRQ indices. */
template <char Port>
struct port_to_pcint_irq;

#define _RAWR_SPECIALIZE_PORT_TO_PCINT_IRQ(port, irq) \
   template <> \
   struct port_to_pcint_irq<port> { \
      static constexpr uint8_t value = irq; \
   };
#ifdef PCINT_A_vect_num
   _RAWR_SPECIALIZE_PORT_TO_PCINT_IRQ('A', PCINT_A_vect_num)
#endif
#ifdef PCINT_B_vect_num
   _RAWR_SPECIALIZE_PORT_TO_PCINT_IRQ('B', PCINT_B_vect_num)
#endif
#ifdef PCINT_C_vect_num
   _RAWR_SPECIALIZE_PORT_TO_PCINT_IRQ('C', PCINT_C_vect_num)
#endif
#ifdef PCINT_D_vect_num
   _RAWR_SPECIALIZE_PORT_TO_PCINT_IRQ('D', PCINT_D_vect_num)
#endif
#undef _RAWR_SPECIALIZE_PORT_TO_PCINT_IRQ

//! Provides access to PCINT registers and bits by PCINT IRQ index.
template <uint8_t Index>
struct port_pcint;

#define _RAWR_SPECIALIZE_PORT_PCINT(index, pcmsk, pcie, pcif) \
   template <> \
   struct port_pcint<index> { \
      static constexpr uint8_t pcint_enable_bit = pcie; \
      static constexpr uint8_t pcint_flag_bit = pcif; \
      static constexpr auto pcint_mask = &pcmsk; \
   };
#ifdef PCIE0
   _RAWR_SPECIALIZE_PORT_PCINT(0, PCMSK0, PCIE0, PCIF0)
#endif
#ifdef PCIE1
   _RAWR_SPECIALIZE_PORT_PCINT(1, PCMSK1, PCIE1, PCIF1)
#endif
#ifdef PCIE2
   _RAWR_SPECIALIZE_PORT_PCINT(2, PCMSK2, PCIE2, PCIF2)
#endif
#ifdef PCIE3
   _RAWR_SPECIALIZE_PORT_PCINT(3, PCMSK3, PCIE3, PCIF3)
#endif
#undef _RAWR_SPECIALIZE_PORT_PCINT

}}} //namespace rawr::hw::_pvt

namespace rawr { namespace hw {

//! Provides access to I/O port registers and bits by port name (“A”, “B”, …).
template <char Port>
struct io_port {
   // Check for just any value known to be false.
   static_assert(!Port, "the selected MCU does not seem to have this port");
};

#define _RAWR_SPECIALIZE_IO_PORT(port, port_unquoted, defined_pins) \
   template <> \
   struct io_port<port> : \
      public _pvt::port_pcint<_pvt::irq_to_pcint_index<_pvt::port_to_pcint_irq<port>::value>::value> { \
      static constexpr auto data_direction = &RAWR_CPP_CAT2(DDR, port_unquoted); \
      static constexpr auto data = &RAWR_CPP_CAT2(PORT, port_unquoted); \
      static constexpr auto pins = &RAWR_CPP_CAT2(PIN, port_unquoted); \
      static constexpr uint8_t bit_size = defined_pins; \
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
#undef _RAWR_SPECIALIZE_IO_PORT

}} //namespace rawr::hw

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

/*! Generates initialization code for the specified I/O port bit as a binary (“digital”) output.

The extra argument allows to provide specializations (below) that are effectively full, but are actually
partial due to the extra template argument, and therefore won’t be instantiated immediately on declaration. */
template <char Port, uint8_t Bit, bool Value, typename = void>
struct io_port_bit_output_setup {
   // Check for just any value known to be false.
   static_assert(!Port, "the selected MCU does not seem to have a pin mapped to this port/bit combination");
};

/*! Generates initialization code for the specified I/O port PCINT.

The extra argument allows to provide specializations (below) that are effectively full, but are actually
partial due to the extra template argument, and therefore won’t be instantiated immediately on declaration. */

template <char Port, typename = void>
struct io_port_input_setup {
   // Check for just any value known to be false.
   static_assert(!Port, "the selected MCU does not seem to have this port");
};

/*! Generates initialization code for the specified I/O port bit as a binary (“digital”) input.

The extra argument allows to provide specializations (below) that are effectively full, but are actually
partial due to the extra template argument, and therefore won’t be instantiated immediately on declaration. */

template <char Port, uint8_t Bit, bool InitialPullup, bool PinChangeInt, typename = void>
struct io_port_bit_input_setup {
   // Check for just any value known to be false.
   static_assert(!Port, "the selected MCU does not seem to have a pin mapped to this port/bit combination");
};

/* TODO: once a client causes one of these templates to be instantiated, make sure that no other template for
the same port/bit can be instantiated, by defining a unique symbol or something. */

#define _RAWR_SPECIALIZE_IO_PORT_PCINT_SETUP(port_ascii) \
   template <typename V> \
   struct io_port_input_setup<port_ascii, V> { \
   private: \
      RAWR_STATIC_CONSTRUCTOR_BEGIN("_ZN4rawr2hw19io_port_input_setupILc" RAWR_TOSTRING(port_ascii) "EvE") \
         typedef io_port<port_ascii> io_port_; \
         bitmanip::set(&PCICR, io_port_::pcint_enable_bit); \
      RAWR_STATIC_CONSTRUCTOR_END() \
   };

#define _RAWR_SPECIALIZE_IO_PORT_BIT(port_ascii, bit) \
   template <typename V> \
   struct io_port_bit_input_setup<port_ascii, bit, false /*pullup*/, false /*PCINT*/, V> { \
      /* No PCINT, and default for DDRxn and PORTxn is 0, so there’s nothing to setup. */ \
   }; \
   \
   template <typename V> \
   struct io_port_bit_input_setup<port_ascii, bit, true /*pullup*/, false /*PCINT*/, V> { \
   private: \
      RAWR_STATIC_CONSTRUCTOR_BEGIN("_ZN4rawr2hw23io_port_bit_input_setupILc" RAWR_TOSTRING(port_ascii) "ELh" RAWR_TOSTRING(bit) "ELb1ELb0EvE") \
         typedef io_port<port_ascii> io_port_; \
         /* Default for DDRxn is 0, so we don’t need to set that. */ \
         bitmanip::set(io_port_::data, bit); \
      RAWR_STATIC_CONSTRUCTOR_END() \
   }; \
   \
   template <typename V> \
   struct io_port_bit_input_setup<port_ascii, bit, false /*pullup*/, true /*PCINT*/, V> : \
      io_port_input_setup<port_ascii, V> { \
   private: \
      RAWR_STATIC_CONSTRUCTOR_BEGIN("_ZN4rawr2hw23io_port_bit_input_setupILc" RAWR_TOSTRING(port_ascii) "ELh" RAWR_TOSTRING(bit) "ELb0ELb1EvE") \
         typedef io_port<port_ascii> io_port_; \
         /* Default for DDRxn and PORTxn is 0, so we don’t need to set those. */ \
         bitmanip::set(io_port_::pcint_mask, bit); \
      RAWR_STATIC_CONSTRUCTOR_END() \
   }; \
   \
   template <typename V> \
   struct io_port_bit_input_setup<port_ascii, bit, true /*pullup*/, true /*PCINT*/, V> : \
      io_port_input_setup<port_ascii, V> { \
   private: \
      RAWR_STATIC_CONSTRUCTOR_BEGIN("_ZN4rawr2hw23io_port_bit_input_setupILc" RAWR_TOSTRING(port_ascii) "ELh" RAWR_TOSTRING(bit) "ELb1ELb1EvE") \
         typedef io_port<port_ascii> io_port_; \
         /* Default for DDRxn is 0, so we don’t need to set that. */ \
         bitmanip::set(io_port_::data, bit); \
         bitmanip::set(io_port_::pcint_mask, bit); \
      RAWR_STATIC_CONSTRUCTOR_END() \
   }; \
   \
   template <typename V> \
   struct io_port_bit_output_setup<port_ascii, bit, false /*value*/, V> { \
   private: \
      RAWR_STATIC_CONSTRUCTOR_BEGIN("_ZN4rawr2hw24io_port_bit_output_setupILc" RAWR_TOSTRING(port_ascii) "ELh" RAWR_TOSTRING(bit) "ELb0EvE") \
         typedef io_port<port_ascii> io_port_; \
         /* Default for PORTxn is 0, so we don’t need to set that. */ \
         bitmanip::set(io_port_::data_direction, bit); \
      RAWR_STATIC_CONSTRUCTOR_END() \
   }; \
   \
   template <typename V> \
   struct io_port_bit_output_setup<port_ascii, bit, true /*value*/, V> { \
   private: \
      RAWR_STATIC_CONSTRUCTOR_BEGIN("_ZN4rawr2hw24io_port_bit_output_setupILc" RAWR_TOSTRING(port_ascii) "ELh" RAWR_TOSTRING(bit) "ELb1EvE") \
         typedef io_port<port_ascii> io_port_; \
         /* Set in this order per Atmel recommendation. */ \
         bitmanip::set(io_port_::data, bit); \
         bitmanip::set(io_port_::data_direction, bit); \
      RAWR_STATIC_CONSTRUCTOR_END() \
   };
#ifdef PORTA
   _RAWR_SPECIALIZE_IO_PORT_PCINT_SETUP(65)
   #ifdef PORTA0
      _RAWR_SPECIALIZE_IO_PORT_BIT(65, 0)
   #endif
   #ifdef PORTA1
      _RAWR_SPECIALIZE_IO_PORT_BIT(65, 1)
   #endif
   #ifdef PORTA2
      _RAWR_SPECIALIZE_IO_PORT_BIT(65, 2)
   #endif
   #ifdef PORTA3
      _RAWR_SPECIALIZE_IO_PORT_BIT(65, 3)
   #endif
   #ifdef PORTA4
      _RAWR_SPECIALIZE_IO_PORT_BIT(65, 4)
   #endif
   #ifdef PORTA5
      _RAWR_SPECIALIZE_IO_PORT_BIT(65, 5)
   #endif
   #ifdef PORTA6
      _RAWR_SPECIALIZE_IO_PORT_BIT(65, 6)
   #endif
   #ifdef PORTA7
      _RAWR_SPECIALIZE_IO_PORT_BIT(65, 7)
   #endif
#endif
#ifdef PORTB
   _RAWR_SPECIALIZE_IO_PORT_PCINT_SETUP(66)
   #ifdef PORTB0
      _RAWR_SPECIALIZE_IO_PORT_BIT(66, 0)
   #endif
   #ifdef PORTB1
      _RAWR_SPECIALIZE_IO_PORT_BIT(66, 1)
   #endif
   #ifdef PORTB2
      _RAWR_SPECIALIZE_IO_PORT_BIT(66, 2)
   #endif
   #ifdef PORTB3
      _RAWR_SPECIALIZE_IO_PORT_BIT(66, 3)
   #endif
   #ifdef PORTB4
      _RAWR_SPECIALIZE_IO_PORT_BIT(66, 4)
   #endif
   #ifdef PORTB5
      _RAWR_SPECIALIZE_IO_PORT_BIT(66, 5)
   #endif
   #ifdef PORTB6
      _RAWR_SPECIALIZE_IO_PORT_BIT(66, 6)
   #endif
   #ifdef PORTB7
      _RAWR_SPECIALIZE_IO_PORT_BIT(66, 7)
   #endif
#endif
#ifdef PORTC
   _RAWR_SPECIALIZE_IO_PORT_PCINT_SETUP(67)
   #ifdef PORTC0
      _RAWR_SPECIALIZE_IO_PORT_BIT(67, 0)
   #endif
   #ifdef PORTC1
      _RAWR_SPECIALIZE_IO_PORT_BIT(67, 1)
   #endif
   #ifdef PORTC2
      _RAWR_SPECIALIZE_IO_PORT_BIT(67, 2)
   #endif
   #ifdef PORTC3
      _RAWR_SPECIALIZE_IO_PORT_BIT(67, 3)
   #endif
   #ifdef PORTC4
      _RAWR_SPECIALIZE_IO_PORT_BIT(67, 4)
   #endif
   #ifdef PORTC5
      _RAWR_SPECIALIZE_IO_PORT_BIT(67, 5)
   #endif
   #ifdef PORTC6
      _RAWR_SPECIALIZE_IO_PORT_BIT(67, 6)
   #endif
   #ifdef PORTC7
      _RAWR_SPECIALIZE_IO_PORT_BIT(67, 7)
   #endif
#endif
#ifdef PORTD
   _RAWR_SPECIALIZE_IO_PORT_PCINT_SETUP(68)
   #ifdef PORTD0
      _RAWR_SPECIALIZE_IO_PORT_BIT(68, 0)
   #endif
   #ifdef PORTD1
      _RAWR_SPECIALIZE_IO_PORT_BIT(68, 1)
   #endif
   #ifdef PORTD2
      _RAWR_SPECIALIZE_IO_PORT_BIT(68, 2)
   #endif
   #ifdef PORTD3
      _RAWR_SPECIALIZE_IO_PORT_BIT(68, 3)
   #endif
   #ifdef PORTD4
      _RAWR_SPECIALIZE_IO_PORT_BIT(68, 4)
   #endif
   #ifdef PORTD5
      _RAWR_SPECIALIZE_IO_PORT_BIT(68, 5)
   #endif
   #ifdef PORTD6
      _RAWR_SPECIALIZE_IO_PORT_BIT(68, 6)
   #endif
   #ifdef PORTD7
      _RAWR_SPECIALIZE_IO_PORT_BIT(68, 7)
   #endif
#endif
#undef _RAWR_SPECIALIZE_IO_PORT_BIT
#undef _RAWR_SPECIALIZE_IO_PORT_PCINT_SETUP

}} //namespace rawr::hw
