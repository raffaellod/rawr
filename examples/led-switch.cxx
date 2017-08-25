/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Switch-toggled LEDs example

This program will toggle pin B3 every time D5 is grounded (defaults to 1 via pull-up), and B4 every time
D4 is grounded. Both D4 and D5 feature a software debouncer using timer_mux. */

/* MCU ATtiny4313 */
#define F_CPU 1000000

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <rawr/binary_input_pin.hxx>
#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uc_main() {
   typedef rawr::hw::binary_output_pin<'B', 3, true /*initialize to logic 1*/> led_b3;
   typedef rawr::hw::binary_output_pin<'B', 4, true /*initialize to logic 1*/> led_b4;
   typedef rawr::binary_input_pin<'D', 4, true /*initialize with pull-up*/> toggle_switch_d4;
   typedef rawr::binary_input_pin<'D', 5, true /*initialize with pull-up*/> toggle_switch_d5;
   typedef rawr::timer_mux<0> timer_mux;

   toggle_switch_d4::set_callback([] (bool value) {
      static bool volatile ignore_next = false;
      if (!ignore_next) {
         if (!value) {
            led_b4::toggle();
         }

         ignore_next = true;
         timer_mux::once(200_ms, [] () {
            ignore_next = false;
         });
      }
   });
   toggle_switch_d5::set_callback([] (bool value) {
      static bool volatile ignore_next = false;
      if (!ignore_next) {
         if (!value) {
            led_b3::toggle();
         }

         ignore_next = true;
         timer_mux::once(200_ms, [] () {
            ignore_next = false;
         });
      }
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
