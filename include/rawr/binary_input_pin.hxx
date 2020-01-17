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
#include <rawr/function.hxx>
#include <rawr/hw/io_port.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace _pvt {

template <char Port>
struct binary_input_port_asm;

#define _RAWR_SPECIALIZE_BINARY_INPUT_PORT_ASM_IMPL(port_ascii, vector, prefix) \
   template <> \
   struct binary_input_port_asm<port_ascii> { \
      static void emit() { \
         RAWR_ALIAS(RAWR_TOSTRING(vector), prefix "EE8__vectorEv"); \
      } \
   };
#define _RAWR_SPECIALIZE_BINARY_INPUT_PORT_ASM(port_ascii, vector) \
   _RAWR_SPECIALIZE_BINARY_INPUT_PORT_ASM_IMPL(port_ascii, vector, "_ZN4rawr4_pvt17binary_input_portILc" RAWR_TOSTRING(port_ascii))
#ifdef PCINT_A_vect
   _RAWR_SPECIALIZE_BINARY_INPUT_PORT_ASM(65 /*A*/, PCINT_A_vect)
#endif
#ifdef PCINT_B_vect
   _RAWR_SPECIALIZE_BINARY_INPUT_PORT_ASM(66 /*B*/, PCINT_B_vect)
#endif
#ifdef PCINT_C_vect
   _RAWR_SPECIALIZE_BINARY_INPUT_PORT_ASM(67 /*C*/, PCINT_C_vect)
#endif
#ifdef PCINT_D_vect
   _RAWR_SPECIALIZE_BINARY_INPUT_PORT_ASM(68 /*D*/, PCINT_D_vect)
#endif
#undef _RAWR_SPECIALIZE_BINARY_INPUT_PORT_ASM

template <char Port>
class binary_input_port {
private:
   typedef hw::io_port<Port> io_port_;
   static constexpr uint8_t bit_size = io_port_::bit_size;

protected:
   static constexpr void set_last_pins_bit(uint8_t bit, bool value) {
      if (value) {
         bitmanip::set(&last_pins, bit);
      } else {
         bitmanip::clear(&last_pins, bit);
      }
   }

private:
   // Weird name to work around g++ name check for interrupt vectors.
   static __attribute__((signal, used)) void __vector() {
      binary_input_port_asm<Port>::emit();
      uint8_t curr_pins = *io_port_::pins, changed_pins = curr_pins ^ last_pins;
      for (uint8_t i = 0; i < RAWR_COUNTOF(pin_changed_callbacks); ++i) {
         if ((changed_pins & _BV(i)) != 0) {
            auto & pin_changed_callback = pin_changed_callbacks[i];
            if (pin_changed_callback) {
               pin_changed_callback((curr_pins & _BV(i)) != 0);
            }
         }
      }
      last_pins = curr_pins;
   }

protected:
   static function<void (bool)> pin_changed_callbacks[bit_size];
   /* Initial values for each bit as assigned by binary_input_pin::set_callback() (from InitialPullup) or
   binary_input_pin_initializer::setup() (from its argument). */
   static uint8_t volatile last_pins;
};

template <char Port>
function<void (bool)> binary_input_port<Port>::pin_changed_callbacks[bit_size];

template <char Port>
/*__attribute__((section(".noinit")))*/ uint8_t volatile binary_input_port<Port>::last_pins;

template <char Port, uint8_t Bit, int InitialPullup>
class binary_input_pin_initializer;

template <char Port, uint8_t Bit>
class binary_input_pin_initializer<Port, Bit, -1> {
private:
   typedef hw::io_port<Port> io_port_;

public:
   static void setup(bool pullup) {
      io_port_::data_direction.clear_bit(Bit);
      if (pullup) {
         io_port_::data.set_bit(Bit);
      } else {
         io_port_::data.clear_bit(Bit);
      }
      io_port_::pcint_mask.set_bit(Bit);
      PCICR.set_bit(io_port_::pcint_enable_bit);
   }
};

template <char Port, uint8_t Bit>
class binary_input_pin_initializer<Port, Bit, true> :
   private hw::io_port_bit_input_setup<Port, Bit, true /*pullup*/, true /*PCINT*/> {
};

template <char Port, uint8_t Bit>
class binary_input_pin_initializer<Port, Bit, false> :
   private hw::io_port_bit_input_setup<Port, Bit, false /*pullup*/, true /*PCINT*/> {
};

}} //namespace rawr::_pvt

namespace rawr {

/*! Configures an I/O port’s pin as input, optionally pre-enabling its pin change interrupt and setting its
pull-up to the value provided via the third template argument. */
template <char Port, uint8_t Bit, int InitialPullup = -1>
class binary_input_pin :
   public _pvt::binary_input_pin_initializer<Port, Bit, InitialPullup>,
   public _pvt::binary_input_port<Port> {
private:
   typedef _pvt::binary_input_port<Port> binary_input_port_;
   typedef hw::io_port<Port> io_port_;

public:
   // Note: this clears the “interrupt occurred” flag for the whole port, not just this pin.
   static void clear() {
      PCIFR.set_bit(io_port_::pcint_flag_bit);
   }

   static void set_callback(function<void (bool)> const & callback) {
      if (InitialPullup != -1) {
         binary_input_port_::set_last_pins_bit(Bit, InitialPullup != 0);
      }
      binary_input_port_::pin_changed_callbacks[Bit] = callback;
   }

   static bool value() {
      return (*io_port_::pins & _BV(Bit)) != 0;
   }
};

} //namespace rawr
