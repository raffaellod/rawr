/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017 Raffaello D. Di Napoli

This file is part of RAWR.

RAWR is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

RAWR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#pragma once

#ifndef _AVR_IO_H_
   #error "include <avr/io.h> before of this file"
#endif

// Fix up a few macros that for some reason have different names in different MCU-specific header files.

#if !defined(PCICR) && defined(GIMSK)
   #define PCICR GIMSK
#endif

#if !defined(PCIE0) && defined(PCIE)
   #define PCIE0 PCIE
#endif

#if !defined(PCIF0) && defined(PCIF)
   #define PCIF0 PCIF
#endif

#if !defined(PCIFR) && defined(GIFR)
   #define PCIFR GIFR
#endif

#if !defined(PCMSK0) && defined(PCMSK)
   #define PCMSK0 PCMSK
#endif

#if !defined(TIMSK) && defined(TIMSK0)
   #define TIMSK TIMSK0
#endif

#if !defined(TIMER0_COMPA_vect) && defined(TIM0_COMPA_vect)
   #define TIMER0_COMPA_vect TIM0_COMPA_vect
#endif

#if !defined(TIMER0_COMPB_vect) && defined(TIM0_COMPB_vect)
   #define TIMER0_COMPB_vect TIM0_COMPB_vect
#endif
