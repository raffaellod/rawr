/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2022 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Variable-frequency LED blinker example

Starting from a frequency of 4 Hz, the LED connected to pin B4 (active high) will be toggled at a frequency
adjustable by grounding D4 (adjust faster) or D5 (adjust slower). */

#define RAWR_FUNCTION_DEFAULT_MAX_LAMBDA_SIZE 12

#include <rawr/binary_input_pin.hxx>
#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uc_main() {
   rawr::hw::binary_output_pin<'B', 4> led{true /*initialize to logic 1*/};
   rawr::binary_input_pin<'D', 4> toggle_switch_d4{true /*initialize with pull-up*/};
   rawr::binary_input_pin<'D', 5> toggle_switch_d5{true /*initialize with pull-up*/};
   rawr::timer_mux<0> timer_mux;
   rawr::timer_mux<0>::delay_control repeat_control;
   auto repeat_period{250_ms};

   auto reschedule{[&] {
      repeat_control = timer_mux.repeat(repeat_period, [&] { led.toggle(); });
   }};
   toggle_switch_d4.set_callback([&] (bool value) {
      if (value) {
         if (repeat_period) {
            repeat_period *= 2;
            if (repeat_period >= timer_mux.min_max_duration) {
               repeat_period = timer_mux.min_max_duration;
            }
         } else {
            repeat_period = 1_ms;
         }
         if (repeat_control) {
            repeat_control.cancel();
         } else {
            led.set();
         }
         reschedule();
      }
   });
   toggle_switch_d5.set_callback([&] (bool value) {
      if (value) {
         repeat_period /= 2;
         if (repeat_control) {
            repeat_control.cancel();
         }
         if (repeat_period) {
            reschedule();
         } else {
            repeat_control.cancel();
            led.clear();
         }
      }
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
