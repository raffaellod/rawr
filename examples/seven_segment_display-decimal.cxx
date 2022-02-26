/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2022 Raffaello D. Di Napoli

This file is distributed under the terms of the Creative Commons Attribution-ShareAlike 4.0 International
(CC BY-SA 4.0) license.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Single 7-segment display counter

This program count from 0 to 9 over and over, showing the number on a 7-segment display connected as listed in
the code, below. The increment will occur every second, while the decimal point will stay on for half a second
every second. */

#include <rawr/hw/binary_output_pin.hxx>
#include <rawr/seven_segment_display.hxx>
#include <rawr/startup.hxx>
#include <rawr/timer_mux.hxx>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uc_main() {
   uint8_t number{0};
   rawr::seven_segment_display<
      rawr::seven_segment_display_decimal_font,
      rawr::hw::io_port_pin<'B', 0>, /* A */
      rawr::hw::io_port_pin<'D', 6>, /* B */
      rawr::hw::io_port_pin<'D', 4>, /* C */
      rawr::hw::io_port_pin<'D', 3>, /* D */
      rawr::hw::io_port_pin<'D', 2>, /* E */
      rawr::hw::io_port_pin<'B', 1>, /* F */
      rawr::hw::io_port_pin<'B', 2>  /* G */
   > display{number};
   rawr::hw::binary_output_pin<'D', 5> dot{true /*initialize to 1*/};
   rawr::timer_mux<0> timer_mux;

   timer_mux.repeat(1000_ms, [&] () {
      if (++number > 9) {
         number = 0;
      }
      display.write(number);
   });
   timer_mux.repeat(500_ms, [&] () {
      dot.toggle();
   });

   sei();
   for (;;) {
      sleep_cpu();
   }
}
