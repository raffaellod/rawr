/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2022 Raffaello D. Di Napoli

This file is part of RAWR.

RAWR is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

RAWR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#pragma once

#include <rawr/hw/binary_output_pin.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace _pvt {

//! Defines the narrowest type for a given size in bytes.
template <unsigned Bytes>
struct uint_t_from_bytes {
   typedef uint32_t type;
};

template <>
struct uint_t_from_bytes<1> {
   typedef uint8_t type;
};

template <>
struct uint_t_from_bytes<2> {
   typedef uint16_t type;
};


/*! Allows to regroup the bits making up an “aggregaged word” into ports, so that all bits in one port may be
changed simultaneously. */
template <typename I, typename... IoPortPins>
struct binary_output_pin_aggregator_impl;

// Base step, selecting bits matching a given I/O port pin.
template <typename I, typename IoPortPin0>
struct binary_output_pin_aggregator_impl<I, IoPortPin0> {
   template <char IfPort>
   static constexpr uint8_t get_mask() {
      if (IoPortPin0::port::name != IfPort) {
         return 0;
      } else {
         return 1 << IoPortPin0::pin;
      }
   }

   template <char IfPort, unsigned Index = 0>
   static constexpr uint8_t slice_value(I value) {
      if (IoPortPin0::port::name != IfPort) {
         return 0;
      } else if ((value & (1 << Index)) == 0) {
         return 0;
      } else {
         return 1 << IoPortPin0::pin;
      }
   }
};

// Recursive step, grouping bits based on the I/O port they correspond to.
template <typename I, typename IoPortPin0, typename... IoPortPins>
struct binary_output_pin_aggregator_impl<I, IoPortPin0, IoPortPins...> {
   template <char IfPort>
   static constexpr uint8_t get_mask() {
      return
         binary_output_pin_aggregator_impl<I, IoPortPin0>::template get_mask<IfPort>() |
         binary_output_pin_aggregator_impl<I, IoPortPins...>::template get_mask<IfPort>();
   }

   template <char IfPort, unsigned Index = 0>
   static constexpr uint8_t slice_value(I value) {
      return
         binary_output_pin_aggregator_impl<I, IoPortPin0>::template slice_value<IfPort, sizeof...(IoPortPins) - Index>(value) |
         binary_output_pin_aggregator_impl<I, IoPortPins...>::template slice_value<IfPort, Index>(value);
   }
};

}} //namespace rawr::_pvt

namespace rawr {

/*! Aggregates a variable number of I/O pins as output, dispatching to each individual pin individual bits
from the desired output word, synchronizing the output across them. */
template <typename... IoPortPins>
class binary_output_pin_aggregator {
public:
   static constexpr auto word_size{sizeof...(IoPortPins)};
   using word_type = typename _pvt::uint_t_from_bytes<(word_size + 7) / 8>::type;

private:
   using impl = _pvt::binary_output_pin_aggregator_impl<word_type, IoPortPins...>;

public:
   constexpr binary_output_pin_aggregator() = default;

   binary_output_pin_aggregator(word_type value) {
      set(value);
   }

   //! Changes all output pins to a new logical value, according to the bits in the argument.
   void set(word_type value) {
      /* First calculate all masks and values, and only then set all pins. This should minimize undesirable
      intermediate states. */
#ifdef PORTA
      auto a_mask{impl::template get_mask<'A'>()};
      auto a_value{impl::template slice_value<'A'>(value)};
#endif
#ifdef PORTB
      auto b_mask{impl::template get_mask<'B'>()};
      auto b_value{impl::template slice_value<'B'>(value)};
#endif
#ifdef PORTC
      auto c_mask{impl::template get_mask<'C'>()};
      auto c_value{impl::template slice_value<'C'>(value)};
#endif
#ifdef PORTD
      auto d_mask{impl::template get_mask<'D'>()};
      auto d_value{impl::template slice_value<'D'>(value)};
#endif
#ifdef PORTE
      auto e_mask{impl::template get_mask<'E'>()};
      auto e_value{impl::template slice_value<'E'>(value)};
#endif
#ifdef PORTF
      auto f_mask{impl::template get_mask<'F'>()};
      auto f_value{impl::template slice_value<'F'>(value)};
#endif
#ifdef PORTG
      auto g_mask{impl::template get_mask<'G'>()};
      auto g_value{impl::template slice_value<'G'>(value)};
#endif
#ifdef PORTH
      auto h_mask{impl::template get_mask<'H'>()};
      auto h_value{impl::template slice_value<'H'>(value)};
#endif
#ifdef PORTJ
      auto j_mask{impl::template get_mask<'J'>()};
      auto j_value{impl::template slice_value<'J'>(value)};
#endif
#ifdef PORTA
      if (a_mask) {
         set_slice(a_mask, a_value, PORTA);
      }
#endif
#ifdef PORTB
      if (b_mask) {
         set_slice(b_mask, b_value, PORTB);
      }
#endif
#ifdef PORTC
      if (c_mask) {
         set_slice(c_mask, c_value, PORTC);
      }
#endif
#ifdef PORTD
      if (d_mask) {
         set_slice(d_mask, d_value, PORTD);
      }
#endif
#ifdef PORTE
      if (e_mask) {
         set_slice(e_mask, e_value, PORTE);
      }
#endif
#ifdef PORTF
      if (f_mask) {
         set_slice(f_mask, f_value, PORTF);
      }
#endif
#ifdef PORTG
      if (g_mask) {
         set_slice(g_mask, g_value, PORTG);
      }
#endif
#ifdef PORTH
      if (h_mask) {
         set_slice(h_mask, h_value, PORTH);
      }
#endif
#ifdef PORTJ
      if (j_mask) {
         set_slice(j_mask, j_value, PORTJ);
      }
#endif
   }

private:
   void set_slice(uint8_t mask, uint8_t value, uint8_t volatile & data_reg) {
      if (mask) {
         data_reg = static_cast<uint8_t>(data_reg & ~mask) | value;
      }
   }
};

} //namespace rawr
