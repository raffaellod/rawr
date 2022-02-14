/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017-2020, 2022 Raffaello D. Di Napoli

This file is part of RAWR.

RAWR is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

RAWR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Wrapper for <avr/io.h>

Includes <avr/io.h>, applying a significant (yet transparent to the application) change to the definition of
all I/O registers.

Aliases µCUs registers defined with inconsistent names, such as providing a PCICR (the more common name) alias
for GIMSK (only used in a handful of µCUs). */

#pragma once

#ifdef _AVR_IO_H_
   #error "this file must be included before <avr/io.h>; might suffice to move all rawr/ includes before all avr/ includes"
#endif
#ifdef _AVR_SFR_DEFS_H_
   #error "this file must be included before any header that pulls in <avr/sfr_defs.h>; might suffice to move all rawr/ includes before all avr/ includes"
#endif
#include <rawr/bitmanip.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <avr/sfr_defs.h>

namespace rawr { namespace hw {

/*! In this unusual class, every method is const because instances are supposed to be declared constexpr so
that they can be defined within a class declaration, and they won’t allocate any memory (desirable since their
storage is an I/O memory register). */
template <unsigned Addr, typename T>
struct io_mem_reg {
   //! Allows to extract the type; mostly used to get its size.
   using type = T;

   io_mem_reg const & operator=(T src) const {
      ref() = src;
      return *this;
   }

   operator T volatile &() const {
      return ref();
   }

   constexpr T volatile & ref() const {
      return *reinterpret_cast<T volatile *>(Addr);
   }

   io_mem_reg const & clear_bit(uint8_t bit) const {
      bitmanip::clear(&ref(), bit);
      return *this;
   }

   io_mem_reg const & set_bit(uint8_t bit) const {
      bitmanip::set(&ref(), bit);
      return *this;
   }

   io_mem_reg const & toggle_bit(uint8_t bit) const {
      bitmanip::toggle(&ref(), bit);
      return *this;
   }
};

}} //namespace rawr::hw

// Now override the MMIO macros.
#undef _MMIO_BYTE
#undef _MMIO_WORD
#undef _MMIO_DWORD
#define _MMIO_BYTE(mem_addr)  (::rawr::hw::io_mem_reg<mem_addr, uint8_t>{})
#define _MMIO_WORD(mem_addr)  (::rawr::hw::io_mem_reg<mem_addr, uint16_t>{})
#define _MMIO_DWORD(mem_addr) (::rawr::hw::io_mem_reg<mem_addr, uint32_t>{})

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>

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

#if !defined(TIMER0_COMPA_vect) && defined(TIMER0_COMP_vect)
   #define TIMER0_COMPA_vect TIMER0_COMP_vect
#endif
#if !defined(TIMER0_COMPA_vect) && defined(TIM0_COMPA_vect)
   #define TIMER0_COMPA_vect TIM0_COMPA_vect
#endif
#if !defined(TIMER0_COMPB_vect) && defined(TIM0_COMPB_vect)
   #define TIMER0_COMPB_vect TIM0_COMPB_vect
#endif

#if !defined(TIMER1_COMPA_vect) && defined(TIMER1_COMP_vect)
   #define TIMER1_COMPA_vect TIMER1_COMP_vect
#endif
#if !defined(TIMER1_COMPA_vect) && defined(TIM1_COMPA_vect)
   #define TIMER1_COMPA_vect TIM1_COMPA_vect
#endif
#if !defined(TIMER1_COMPB_vect) && defined(TIM1_COMPB_vect)
   #define TIMER1_COMPB_vect TIM1_COMPB_vect
#endif

#if !defined(TIMER2_COMPA_vect) && defined(TIMER2_COMP_vect)
   #define TIMER2_COMPA_vect TIMER2_COMP_vect
#endif
#if !defined(TIMER2_COMPA_vect) && defined(TIM2_COMPA_vect)
   #define TIMER2_COMPA_vect TIM2_COMPA_vect
#endif
#if !defined(TIMER2_COMPB_vect) && defined(TIM2_COMPB_vect)
   #define TIMER2_COMPB_vect TIM2_COMPB_vect
#endif

#if !defined(WDTCR) && defined(WDTCSR)
   #define WDTCR WDTCSR
#endif
