/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2022 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Single 7-segment display alphanumeric enumerator

This program cycles letters and digits, showing them on a 7-segment display connected as listed in the code,
below. The increment will occur every second. */

/* MCU ATtiny4313 */
#define F_CPU 1000000

#include <rawr/seven_segment_display.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void uc_main() {
   static char const chars[]{" abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
   uint8_t char_index{0};
   rawr::seven_segment_display<
      rawr::seven_segment_display_alphanum_font,
      rawr::hw::io_port_pin<'B', 0>, /* A */
      rawr::hw::io_port_pin<'D', 6>, /* B */
      rawr::hw::io_port_pin<'D', 4>, /* C */
      rawr::hw::io_port_pin<'D', 3>, /* D */
      rawr::hw::io_port_pin<'D', 2>, /* E */
      rawr::hw::io_port_pin<'B', 1>, /* F */
      rawr::hw::io_port_pin<'B', 2>  /* G */
   > display{chars[char_index]};
   rawr::timer_mux<0> timer_mux;

   timer_mux.repeat(250_ms, [&] () {
      if (++char_index >= sizeof chars / sizeof chars[0]) {
         char_index = 0;
      }
      display.write(chars[char_index]);
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
