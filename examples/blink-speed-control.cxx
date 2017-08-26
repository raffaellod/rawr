/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Variable-frequency LED blinker example

Starting from a frequency of 4 Hz, the LED connected to pin B4 (active high) will be toggled at a frequency
adjustable by grounding D4 (adjust faster) or D5 (adjust slower). */

/* MCU ATtiny4313 */
#define F_CPU 1000000

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <rawr/binary_input_pin.hxx>
#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef rawr::hw::binary_output_pin<'B', 4, true /*initialize to logic 1*/> led;
typedef rawr::timer_mux<0> timer_mux;

static rawr::chrono::milliseconds repeat_period{250_ms};
static timer_mux::delay_control repeat_control;

static void reschedule() {
   repeat_control = timer_mux::repeat(repeat_period, &led::toggle);
}

void uc_main() {
   typedef rawr::binary_input_pin<'D', 4, true /*initialize with pull-up*/> toggle_switch_d4;
   typedef rawr::binary_input_pin<'D', 5, true /*initialize with pull-up*/> toggle_switch_d5;

   repeat_control = timer_mux::repeat(repeat_period, &led::toggle);

   toggle_switch_d4::set_callback([] (bool value) {
      if (value) {
         if (repeat_period) {
            repeat_period *= 2;
            if (repeat_period >= timer_mux::min_max_duration) {
               repeat_period = timer_mux::min_max_duration;
            }
         } else {
            repeat_period = 1_ms;
         }
         if (repeat_control) {
            repeat_control.cancel();
         } else {
            led::set();
         }
         reschedule();
      }
   });
   toggle_switch_d5::set_callback([] (bool value) {
      if (value) {
         repeat_period /= 2;
         if (repeat_control) {
            repeat_control.cancel();
         }
         if (repeat_period) {
            reschedule();
         } else {
            repeat_control = timer_mux::delay_control();
            led::clear();
         }
      }
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
