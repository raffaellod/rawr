/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2022 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Variable-brightness LED example

Starting from 50% brightness, the LED connected to pin B4 (active high) can be made 10% brighter by grounding
pin D4, or 10% more dim by grounding pin D5. */

#define RAWR_FUNCTION_DEFAULT_MAX_LAMBDA_SIZE 16

#include <rawr/binary_input_pin.hxx>
#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uc_main() {
   rawr::binary_input_pin<'D', 4> toggle_switch_d4{true /*initialize with pull-up*/};
   rawr::binary_input_pin<'D', 5> toggle_switch_d5{true /*initialize with pull-up*/};
   rawr::hw::binary_output_pin<'B', 4> led{true /*initialize to logic 1*/};
   rawr::timer_mux<0> timer_mux;
   rawr::timer_mux<0>::delay_control off_repeat_control;

   constexpr auto repeat_period{100_ms}, change_delta{10_ms};
   bool reschedule_now{true}, rescheduling{false};
   auto off_delay{50_ms};

   toggle_switch_d4.set_callback([&] (bool value) {
      if (value && !reschedule_now && !rescheduling) {
         off_delay += change_delta;
         reschedule_now = true;
      }
   });
   toggle_switch_d5.set_callback([&] (bool value) {
      if (value && !reschedule_now && !rescheduling) {
         off_delay -= change_delta;
         reschedule_now = true;
      }
   });
   timer_mux.repeat(repeat_period, [&] () {
      if (reschedule_now && !rescheduling) {
         rescheduling = true;
         reschedule_now = false;
         timer_mux.once(off_delay, [&] () {
            off_repeat_control.cancel();
            off_repeat_control = timer_mux.repeat(repeat_period, [&] { led.clear(); });
            rescheduling = false;
         });
      }
      led.set();
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
