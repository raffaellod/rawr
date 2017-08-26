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

#include <rawr/misc.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace chrono {

/* Hertz is never plural: <https://www.nist.gov/pml/
nist-guide-si-chapter-9-rules-and-style-conventions-spelling-unit-names> */
class hertz {
public:
   typedef uint64_t count_type;

public:
   explicit constexpr hertz(count_type count__ = 0) :
      count_(count__) {
   }

   explicit constexpr operator bool() const {
      return count_ != 0;
   }

   constexpr hertz operator+(count_type addend) const {
      return hertz(count_ + addend);
   }

   constexpr hertz operator/(count_type divisor) const {
      return hertz(count_ / divisor);
   }

   constexpr count_type count() const {
      return count_;
   }

private:
   count_type count_;
};

}} //namespace rawr::chrono

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace chrono {

template <typename Int, Int Scale>
class time_unit {
public:
   typedef Int count_type;

public:
   explicit constexpr time_unit(count_type count__ = 0) :
      count_(count__) {
   }

   explicit constexpr operator bool() const {
      return count_ != 0;
   }

   constexpr time_unit & operator+=(time_unit addend) {
      count_ += addend.count_;
      return *this;
   }

   constexpr time_unit & operator-=(time_unit subtrahend) {
      count_ -= subtrahend.count_;
      return *this;
   }

   constexpr time_unit & operator*=(count_type multiplier) {
      count_ *= multiplier;
      return *this;
   }

   constexpr time_unit & operator/=(count_type divisor) {
      count_ /= divisor;
      return *this;
   }

   constexpr time_unit operator+(time_unit addend) const {
      return time_unit(count_ + addend.count_);
   }

   constexpr time_unit operator-(time_unit subtrahend) const {
      return time_unit(count_ - subtrahend.count_);
   }

   constexpr time_unit operator*(count_type multiplier) const {
      return time_unit(count_ * multiplier);
   }

   constexpr time_unit operator/(count_type divisor) const {
      return time_unit(count_ / divisor);
   }

   #define _RAWR_RELOP(op) \
      constexpr bool operator op(time_unit const & other) const { \
         return count_ op other.count_; \
      }
   _RAWR_RELOP(==)
   _RAWR_RELOP(!=)
   _RAWR_RELOP(>=)
   _RAWR_RELOP(<=)
   _RAWR_RELOP(>)
   _RAWR_RELOP(<)
   #undef _RAWR_RELOP

   constexpr count_type count() const {
      return count_;
   }

private:
   count_type count_;
};

// The associated integer types are large enough to hold about 1 minute.
typedef time_unit< uint8_t,          1> seconds;
typedef time_unit<uint16_t,       1000> milliseconds;
typedef time_unit<uint32_t,    1000000> microseconds;
typedef time_unit<uint64_t, 1000000000> nanoseconds;

}} //namespace rawr::chrono

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace chrono {

inline constexpr nanoseconds operator/(uint64_t dividend, hertz hz) {
   return nanoseconds(dividend / hz.count());
}

}} //namespace rawr::chrono

namespace rawr {

template <>
inline constexpr uint64_t int_round_div(uint64_t dividend, chrono::hertz divisor) {
   return (dividend + divisor.count() / 2) / divisor.count();
}

} //namespace rawr

constexpr rawr::chrono::seconds operator""_s(unsigned long long s) {
   return rawr::chrono::seconds(static_cast<rawr::chrono::seconds::count_type>(s));
}

constexpr rawr::chrono::milliseconds operator""_ms(unsigned long long ms) {
   return rawr::chrono::milliseconds(static_cast<rawr::chrono::milliseconds::count_type>(ms));
}

constexpr rawr::chrono::microseconds operator""_us(unsigned long long us) {
   return rawr::chrono::microseconds(static_cast<rawr::chrono::microseconds::count_type>(us));
}

constexpr rawr::chrono::nanoseconds operator""_ns(unsigned long long ns) {
   return rawr::chrono::nanoseconds(static_cast<rawr::chrono::nanoseconds::count_type>(ns));
}
