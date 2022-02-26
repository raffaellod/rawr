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

struct binary_input_pin_data {
   function<void (bool)> changed_callback;
};

template <char Port>
class binary_input_port {
private:
   typedef hw::io_port<Port> io_port_;
   static constexpr unsigned bit_size{io_port_::bit_size};

protected:
   static void set_last_pins_bit(uint8_t bit) {
      if ((io_port_::pins & _BV(bit)) != 0) {
         bitmanip::set(&last_pins, bit);
      } else {
         bitmanip::clear(&last_pins, bit);
      }
   }

private:
   // Weird name to work around g++ name check for interrupt vectors.
   static __attribute__((signal, used)) void __vector() {
      binary_input_port_asm<Port>::emit();
      uint8_t curr_pins = io_port_::pins, changed_pins = curr_pins ^ last_pins;
      for (uint8_t i = 0; i < RAWR_COUNTOF(per_pin_data); ++i) {
         if ((changed_pins & _BV(i)) == 0) {
            // This pin did not really change.
            continue;
         }
         auto pin_data{per_pin_data[i]};
         if (pin_data && pin_data->changed_callback) {
            pin_data->changed_callback((curr_pins & _BV(i)) != 0);
         }
      }
      last_pins = curr_pins;
   }

protected:
   static binary_input_pin_data * per_pin_data[bit_size];
   /*! Since all pins on a port share the same pin change interrupt, we need to track the last state of each
   pin to avoid invoking changed callbacks for pins that did not, in fact, change. */
   static uint8_t volatile last_pins;
};

template <char Port>
binary_input_pin_data * binary_input_port<Port>::per_pin_data[bit_size];

template <char Port>
uint8_t volatile binary_input_port<Port>::last_pins;

}} //namespace rawr::_pvt

namespace rawr {

/*! Configures an I/O port’s pin as input, optionally pre-enabling its pin change interrupt and setting its
pull-up to the value provided via the third template argument. */
template <char Port, uint8_t Bit>
class binary_input_pin : public _pvt::binary_input_port<Port>, protected _pvt::binary_input_pin_data {
private:
   typedef _pvt::binary_input_port<Port> binary_input_port_;
   typedef hw::io_port<Port> io_port_;

public:
   //! Configures the pin as input, without activating internal pull-up or pull-down.
   binary_input_pin() {
      io_port_::data_direction.clear_bit(Bit);
   }

   //! Configures the pin as input, activating internal pull-up or pull-down.
   binary_input_pin(bool pullup) :
      binary_input_pin{} {
      if (pullup) {
         io_port_::data.set_bit(Bit);
      } else {
         io_port_::data.clear_bit(Bit);
      }
   }

   // Note: this clears the “interrupt occurred” flag for the whole port, not just this pin.
   void clear() {
      PCIFR.set_bit(io_port_::pcint_flag_bit);
   }

   template <typename F>
   void set_callback(F callback) {
      binary_input_port_::set_last_pins_bit(Bit);
      changed_callback = move(callback);
      io_port_::pcint_mask.set_bit(Bit);
      PCICR.set_bit(io_port_::pcint_enable_bit);
   }

   bool value() const {
      return (io_port_::pins & _BV(Bit)) != 0;
   }
};

} //namespace rawr
