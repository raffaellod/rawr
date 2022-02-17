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

#include <rawr/binary_output_pin_aggregator.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

//! Allows to define a 7-segment display character in a visual way.
#define RAWR_1_7SEGS( \
      a,              \
    f,  b,            \
      g,              \
    e,  c,            \
      d               \
)                     \
   static_cast<uint8_t>((a << 6) | (b << 5) | (c << 4) | (d << 3) | (e << 2) | (f << 1) | g)

//! Same as RAWR_1_7SEGS(), but five at a time.
#define RAWR_5_7SEGS(                       \
      a0,     a1,     a2,     a3,     a4,   \
    f0, b0, f1, b1, f2, b2, f3, b3, f4, b4, \
      g0,     g1,     g2,     g3,     g4,   \
    e0, c0, e1, c1, e2, c2, e3, c3, e4, c4, \
      d0,     d1,     d2,     d3,     d4    \
)                                           \
   RAWR_1_7SEGS(a0, f0, b0, g0, e0, c0, d0), \
   RAWR_1_7SEGS(a1, f1, b1, g1, e1, c1, d1), \
   RAWR_1_7SEGS(a2, f2, b2, g2, e2, c2, d2), \
   RAWR_1_7SEGS(a3, f3, b3, g3, e3, c3, d3), \
   RAWR_1_7SEGS(a4, f4, b4, g4, e4, c4, d4)

//! Provides characters to display numbers from 0 to 9.
struct seven_segment_display_decimal_font {
   using source_type = uint8_t;

   uint8_t map(source_type source) const {
      static constexpr uint8_t const digits[]{
         RAWR_5_7SEGS(/*0*/  1,   /*1*/  0,   /*2*/  1,   /*3*/  1,   /*4*/  0,
                           1,  1,      0,  1,      0,  1,      0,  1,      1,  1,
                             0,          0,          1,          1,          1,
                           1,  1,      0,  1,      1,  0,      0,  1,      0,  1,
                             1,          0,          1,          1,          0   ),

         RAWR_5_7SEGS(/*5*/  1,   /*6*/  1,   /*7*/  1,   /*8*/  1,   /*9*/  1,
                           1,  0,      1,  0,      0,  1,      1,  1,      1,  1,
                             1,          1,          0,          1,          1,
                           0,  1,      1,  1,      0,  1,      1,  1,      0,  1,
                             1,          1,          0,          1,          1   )
      };
      return source < sizeof digits / sizeof digits[0] ? digits[source] : 0;
   }
};

/*! Provides characters to display numbers from 0 to 9 and letters. While both upper- and lower-case letters
are supported, they’re all rendered in a mixed upper/lower case, depending on which looks better. */
struct seven_segment_display_alphanum_font : protected seven_segment_display_decimal_font {
   using source_type = char;

   uint8_t map(source_type source) const {
      static constexpr uint8_t const letters[]{
         RAWR_5_7SEGS(/*A*/  1,   /*b*/  0,   /*C*/  1,   /*d*/  0,   /*E*/  1,
                           1,  1,      1,  0,      1,  0,      0,  1,      1,  0,
                             1,          1,          0,          1,          1,
                           1,  1,      1,  1,      1,  0,      1,  1,      1,  0,
                             0,          1,          1,          1,          1   ),

         RAWR_5_7SEGS(/*F*/  1,   /*G*/  1,   /*H*/  0,   /*i*/  0,   /*J*/  0,
                           1,  0,      1,  0,      1,  1,      0,  0,      0,  1,
                             1,          0,          1,          0,          0,
                           1,  0,      1,  1,      1,  1,      0,  1,      1,  1,
                             0,          1,          0,          0,          1   ),

         RAWR_5_7SEGS(/*K*/  0,   /*L*/  0,   /*m*/  1,   /*n*/  0,   /*o*/  0,
                           1,  0,      1,  0,      0,  0,      0,  0,      0,  0,
                             1,          0,          0,          1,          1,
                           1,  1,      1,  0,      1,  1,      1,  1,      1,  1,
                             0,          1,          0,          0,          1   ),

         RAWR_5_7SEGS(/*P*/  1,   /*q*/  1,   /*r*/  0,   /*S*/  1,   /*t*/  0,
                           1,  1,      1,  1,      0,  0,      1,  0,      1,  0,
                             1,          1,          1,          1,          1,
                           1,  0,      0,  1,      1,  0,      0,  1,      1,  0,
                             0,          0,          0,          1,          1   ),

         RAWR_5_7SEGS(/*U*/  0,   /*V*/  0,   /*w*/  0,   /*x*/  0,   /*y*/  0,
                           1,  1,      1,  1,      1,  1,      0,  0,      1,  1,
                             0,          0,          0,          0,          1,
                           1,  1,      0,  1,      0,  0,      1,  1,      0,  1,
                             1,          1,          1,          0,          1   ),

         RAWR_1_7SEGS(/*Z*/  1,
                           0,  1,
                             1,
                           1,  0,
                             1   )
      };
      if (source >= '0' && source <= '9') {
         return seven_segment_display_decimal_font::map(
            static_cast<seven_segment_display_decimal_font::source_type>(source - '0')
         );
      } else if (source >= 'A' && source <= 'Z') {
         return letters[source - 'A'];
      } else if (source >= 'a' && source <= 'z') {
         return letters[source - 'a'];
      } else {
         return 0;
      }
   }
};


//! Output generator for a 7-segment display. The decimal point is not managed by this class.
template <
   typename Font,
   typename IoPortPinA, typename IoPortPinB, typename IoPortPinC, typename IoPortPinD,
   typename IoPortPinE, typename IoPortPinF, typename IoPortPinG
>
class seven_segment_display :
   protected binary_output_pin_aggregator<
      IoPortPinA, IoPortPinB, IoPortPinC, IoPortPinD, IoPortPinE, IoPortPinF, IoPortPinG
   >,
   protected Font {
public:
   using font_type = Font;

protected:
   using aggregator_type = binary_output_pin_aggregator<
      IoPortPinA, IoPortPinB, IoPortPinC, IoPortPinD, IoPortPinE, IoPortPinF, IoPortPinG
   >;

public:
   constexpr seven_segment_display() = default;
   constexpr seven_segment_display(typename Font::source_type value) :
      aggregator_type{font().map(value)} {
   }

   font_type & font() {
      return *this;
   }
   font_type const & font() const {
      return *this;
   }

   void write(typename Font::source_type value) {
      aggregator().set(font().map(value));
   }

protected:
   aggregator_type & aggregator() {
      return *this;
   }
};

} //namespace rawr
