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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

//! Resets the device.
[[noreturn]] __attribute__((naked, weak)) void reset();

void reset() {
   __asm__ __volatile__(
      "\r\n" "cli"
   );
   WDTCR |= _BV(WDCE) | _BV(WDE);
   // Set prescaler to 0000, which is the shortest period.
   WDTCR = _BV(WDCE);
   // Sleep with interrupts disabled till reset occurs.
   for (;;) {
   }
}

} //namespace rawr
