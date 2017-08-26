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

#include <avr/wdt.h>
#include <rawr/hw/consistent_names.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

class watchdog_timer {
public:
   enum class timeout {
      _8s    = 0b1001,
      max    = _8s,
      _4s    = 0b1000,
      _2s    = 0b0111,
      _1s    = 0b0110,
      _500ms = 0b0101,
      _250ms = 0b0100,
      _125ms = 0b0011,
      _64ms  = 0b0010,
      _32ms  = 0b0001,
      _16ms  = 0b0000,
      min    = _16ms
   };

public:
   static void disable() {
      wdt_disable();
   }

   static void enable(enum timeout timeout) {
      wdt_enable(static_cast<uint8_t>(timeout));
   }

   static void reset() {
      wdt_reset();
   }
};

}} //namespace rawr::hw
