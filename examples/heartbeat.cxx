/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2022 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
LED blinker example

This program will toggle pin B3 twice every second, making an LED (if one is connected via a resistor between
B3 and Vcc or GND) blink in a way resemblant of a heartbeat. */

#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uc_main() {
   rawr::hw::binary_output_pin<'B', 3> led{true /*initialize to logic 1*/};
   rawr::timer_mux<0> timer_mux;
   timer_mux.repeat(1000_ms, [&] () {
      led.set();
      timer_mux.once(100_ms, [&] () {
         led.clear();
      });
      timer_mux.once(200_ms, [&] () {
         led.set();
      });
      timer_mux.once(300_ms, [&] () {
         led.clear();
      });
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
