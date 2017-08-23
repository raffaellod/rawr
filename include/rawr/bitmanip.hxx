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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr { namespace bitmanip {

template <typename T>
inline constexpr void clear(T * t, uint8_t bit) {
   *t = static_cast<T>(*t & static_cast<T>(~static_cast<T>(1 << bit)));
}
template <typename T, typename... Ts>
inline constexpr void clear(T * t, uint8_t bit, Ts... more_bits) {
   return clear(t, bit) | clear(t, more_bits...);
}

template <typename T>
inline constexpr void set(T * t, uint8_t bit) {
   *t = static_cast<T>(*t | static_cast<T>(1 << bit));
}
template <typename T, typename... Ts>
inline constexpr void set(T * t, uint8_t bit, Ts... more_bits) {
   return set(t, bit) | set(t, more_bits...);
}

template <typename T>
inline constexpr void toggle(T * t, uint8_t bit) {
   *t = static_cast<T>(*t ^ static_cast<T>(1 << bit));
}

}} //namespace rawr::bitmanip
