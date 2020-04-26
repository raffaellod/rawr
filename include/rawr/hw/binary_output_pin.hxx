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

#include <rawr/hw/io_port.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

//! Configures an I/O port’s pin as output.
template <char Port, uint8_t Bit>
class binary_output_pin {
private:
   typedef io_port<Port> io_port_;

public:
   binary_output_pin(bool value = false) {
      // Default for PORTxn is 0, so we only need to do this if value is true (1).
      if (value) {
         set(value);
      }
      io_port_::data_direction.set_bit(Bit);
   }

   binary_output_pin(binary_output_pin const &) = delete;

   binary_output_pin & operator=(binary_output_pin const &) = delete;

   void set(bool value = true) {
      if (value) {
         io_port_::data.set_bit(Bit);
      } else {
         io_port_::data.clear_bit(Bit);
      }
   }

   void clear() {
      set(false);
   }

   void toggle() {
      io_port_::data.toggle_bit(Bit);
   }
};

}} //namespace rawr::hw
