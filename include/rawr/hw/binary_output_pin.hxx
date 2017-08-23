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

#include <rawr/bitmanip.hxx>
#include <rawr/hw/io_port.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw { namespace _pvt {

template <char Port, uint8_t Bit, int InitialValue>
class binary_output_pin_initializer;

template <char Port, uint8_t Bit>
class binary_output_pin_initializer<Port, Bit, -1> {
private:
   typedef io_port<Port> io_port_;

public:
   static void setup(bool value = false) {
      if (value) {
         bitmanip::set(io_port_::data, Bit);
      } else {
         bitmanip::clear(io_port_::data, Bit);
      }
      bitmanip::set(io_port_::data_direction, Bit);
   }
};

template <char Port, uint8_t Bit>
class binary_output_pin_initializer<Port, Bit, true> : private io_port_bit_output_setup<Port, Bit, true> {
};

template <char Port, uint8_t Bit>
class binary_output_pin_initializer<Port, Bit, false> : private io_port_bit_output_setup<Port, Bit, false> {
};

}}} //namespace rawr::hw::_pvt

namespace rawr { namespace hw {

/*! Configures an I/O port’s pin as output, optionally pre-setting its state to the value provided via the
third template argument. */
template <char Port, uint8_t Bit, int InitialValue = -1>
class binary_output_pin : public _pvt::binary_output_pin_initializer<Port, Bit, InitialValue> {
private:
   typedef io_port<Port> io_port_;

public:
   static void set(bool value = true) {
      if (value) {
         bitmanip::set(io_port_::data, Bit);
      } else {
         bitmanip::clear(io_port_::data, Bit);
      }
   }

   static void clear() {
      set(false);
   }

   static void toggle() {
      bitmanip::toggle(io_port_::data, Bit);
   }
};

}} //namespace rawr::hw
