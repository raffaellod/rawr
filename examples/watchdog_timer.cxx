/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2022 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Watchdog timer example

This program lets the user turn on or off an LED (on B4, active high) by grounding D4 or D5 (active low) while
the watchdog timer is running. After 4 seconds of inactivity, the microcontroller is reset. */

#include <rawr/binary_input_pin.hxx>
#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/hw/watchdog_timer.hxx>
#include <rawr/startup.hxx>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uc_main() {
   rawr::hw::binary_output_pin<'B', 4> led{false /*initialize to logic 0*/};
   rawr::binary_input_pin<'D', 4> button_d4{true /*initialize with pull-up*/};
   rawr::binary_input_pin<'D', 5> button_d5{true /*initialize with pull-up*/};
   rawr::hw::watchdog_timer wdt;

   // Reset after 4 seconds of inactivity.
   wdt.enable(wdt.timeout::_4s);

   button_d4.set_callback([&] (bool) {
      led.set();
      wdt.reset();
   });
   button_d5.set_callback([&] (bool) {
      led.clear();
      wdt.reset();
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
