/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2022 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
LED blinker example

This program will toggle pin B3 every 250 ms, making an LED blink if one is connected via a resistor between
B3 and Vcc or GND. */

#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uc_main() {
   rawr::hw::binary_output_pin<'B', 3> led{true /*initialize to logic 1*/};
   rawr::timer_mux<0> timer_mux;

   timer_mux.repeat(250_ms, [&] () {
      led.toggle();
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
