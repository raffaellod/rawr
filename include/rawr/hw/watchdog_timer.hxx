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
#include <avr/wdt.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace hw {

class watchdog_timer {
public:
   enum class timeout {
      _16ms  = 0x0,
      _32ms  = 0x1,
      _64ms  = 0x2,
      _125ms = 0x3,
      _250ms = 0x4,
      _500ms = 0x5,
      _1s    = 0x6,
      _2s    = 0x7,
      _4s    = 0x8,
      _8s    = 0x9,
      max    = _8s,
      min    = _16ms
   };

public:
   void disable() {
      wdt_disable();
   }

   void enable(enum timeout timeout) {
      wdt_enable(static_cast<uint8_t>(timeout));
   }

   void reset() {
      wdt_reset();
   }
};

}} //namespace rawr::hw
