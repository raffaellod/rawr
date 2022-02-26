/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2022 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Switch-toggled LEDs example

This program will toggle pin B3 every time D5 is grounded (defaults to 1 via pull-up), and B4 every time
D4 is grounded. Both D4 and D5 feature a software debouncer using timer_mux. */

#define RAWR_FUNCTION_DEFAULT_MAX_LAMBDA_SIZE 8

#include <rawr/binary_input_pin.hxx>
#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uc_main() {
   rawr::hw::binary_output_pin<'B', 3> led_b3{true /*initialize to logic 1*/};
   rawr::hw::binary_output_pin<'B', 4> led_b4{true /*initialize to logic 1*/};
   rawr::binary_input_pin<'D', 4> toggle_switch_d4{true /*initialize with pull-up*/};
   rawr::binary_input_pin<'D', 5> toggle_switch_d5{true /*initialize with pull-up*/};
   rawr::timer_mux<0> timer_mux;

   bool ignore_next_d4{false}, ignore_next_d5{false};
   toggle_switch_d4.set_callback([&] (bool value) {
      if (!ignore_next_d4) {
         ignore_next_d4 = true;
         if (!value) {
            led_b4.toggle();
         }

         timer_mux.once(200_ms, [&] () {
            ignore_next_d4 = false;
         });
      }
   });
   toggle_switch_d5.set_callback([&] (bool value) {
      if (!ignore_next_d5) {
         if (!value) {
            led_b3.toggle();
         }

         ignore_next_d5 = true;
         timer_mux.once(200_ms, [&] () {
            ignore_next_d5 = false;
         });
      }
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
