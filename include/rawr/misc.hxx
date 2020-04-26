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

#define _RAWR_TOSTRING(x) #x
#define RAWR_TOSTRING(x) _RAWR_TOSTRING(x)

#define _RAWR_CPP_CAT2(s1, s2) s1 ## s2
#define RAWR_CPP_CAT2(s1, s2) _RAWR_CPP_CAT2(s1, s2)

#define _RAWR_CPP_CAT3(s1, s2, s3) s1 ## s2 ## s3
#define RAWR_CPP_CAT3(s1, s2, s3) _RAWR_CPP_CAT3(s1, s2, s3)

#define _RAWR_CPP_CAT4(s1, s2, s3, s4) s1 ## s2 ## s3 ## s4
#define RAWR_CPP_CAT4(s1, s2, s3, s4) _RAWR_CPP_CAT4(s1, s2, s3, s4)

/*! Returns the number of items in a (static) array.

@param array
   Array for which to compute the count of items.
@return
   Count of items in array.
*/
#define RAWR_COUNTOF(array) \
   (sizeof(array) / sizeof((array)[0]))

#ifdef __AVR_HAVE_JMP_CALL__
   #define RAWR_ASM_CALL "call"
   #define RAWR_ASM_JMP  "jmp"
#else
   #define RAWR_ASM_CALL "rcall"
   #define RAWR_ASM_JMP  "rjmp"
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Placement new and delete (C++11 § 18.6.1.3 “Placement forms”).
inline void * operator new(decltype(sizeof 0), void * p) {
   return p;
}

inline void * operator new[](decltype(sizeof 0), void * p) {
   return p;
}

inline void operator delete(void *, void *) {
}

inline void operator delete[](void *, void *) {
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

//! Defines a member named type as T, if and only if test == true (C++11 § 20.9.7.6 “Other transformations”).
template <bool Test, typename T = void>
struct enable_if {
};
template <typename T>
struct enable_if<true, T> {
   typedef T type;
};

} //namespace rawr

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

//! Defines a member named type as T.
template <typename T>
struct identity {
   typedef T type;
};

} //namespace rawr

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

//! Removes const and volatile qualifiers from a type (C++11 § 20.9.7.1 “Const-volatile modifications”).
template <typename T>
struct remove_cv {
   typedef T type;
};
template <typename T>
struct remove_cv<T const> {
   typedef T type;
};
template <typename T>
struct remove_cv<T volatile> {
   typedef T type;
};
template <typename T>
struct remove_cv<T const volatile> {
   typedef T type;
};

} //namespace rawr

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

/*! Defines a member named type as T, removing reference qualifiers if any are present (C++11 § 20.9.7.2
“Reference modifications”). */
template <typename T>
struct remove_reference {
   typedef T type;
};
template <typename T>
struct remove_reference<T &> {
   typedef T type;
};
template <typename T>
struct remove_reference<T &&> {
   typedef T type;
};

} //namespace rawr

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

/*! Defined as _std::true_type if T is an l-value reference type, or _std::false_type otherwise (C++11 §
20.9.4.1 “Primary type categories”). */
template <typename T>
struct is_lvalue_reference { static constexpr auto value{false}; };
template <typename T>
struct is_lvalue_reference<T &> { static constexpr auto value{true}; };

} //namespace rawr

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

/*! Allows a method to accept an argument of any reference type, while declaring only an r-value reference
(C++11 § 20.2.3 “forward/move helpers”). N2835-compliant.

@param t
   Source object.
@return
   R-value reference to t.
*/
// Forward r-values as r-values.
template <typename T>
inline typename enable_if<!is_lvalue_reference<T>::value, T &&>::type forward(
   typename identity<T>::type && t
) {
   return static_cast<T &&>(t);
}
// Forward l-values as l-values.
template <typename T>
inline typename enable_if<is_lvalue_reference<T>::value, T>::type forward(typename identity<T>::type t) {
   return t;
}
// Forward l-values as r-values.
template <typename T>
inline typename enable_if<!is_lvalue_reference<T>::value, T &&>::type forward(
   typename identity<T>::type & t
) {
   return static_cast<T &&>(t);
}
// Prevent forwarding r-values as l-values.
template <typename T>
inline typename enable_if<is_lvalue_reference<T>::value, T>::type forward(
   typename remove_reference<T>::type && t
) = delete;

/*! Converts a value into an r-value reference, enabling move semantics on the argument (C++11 § 20.2.3
“forward/move helpers”).

@param t
   Source object.
@return
   R-value reference to t.
*/
template <typename T>
inline constexpr typename remove_reference<T>::type && move(T && t) {
   return static_cast<typename remove_reference<T>::type &&>(t);
}

} //namespace rawr

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

/*! Returns the greatest of two objects (C++11 § 25.4.7 “Minimum and maximum”).

@param t1
   First value.
@param t2
   Second value.
@return
   Greatest of t1 and t2.
*/
template <typename T>
inline constexpr T max(T t1, T t2) {
   return t1 > t2 ? t1 : t2;
}
template <typename T, class TCompare>
inline constexpr T max(T t1, T t2, TCompare compare_fn) {
   return compare_fn(t1, t2) > 0 ? t1 : t2;
}

} //namespace rawr

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

/*! Returns the least of two objects (C++11 § 25.4.7 “Minimum and maximum”).

@param t1
   First value.
@param t2
   Second value.
@return
   Least of t1 and t2.
*/
template <typename T>
inline constexpr T min(T t1, T t2) {
   return t1 < t2 ? t1 : t2;
}
template <typename T, class TCompare>
inline constexpr T min(T t1, T t2, TCompare compare_fn) {
   return compare_fn(t1, t2) < 0 ? t1 : t2;
}

} //namespace rawr

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

template <typename T1, typename T2>
inline constexpr T1 int_round_div(T1 dividend, T2 divisor) {
   static_assert(sizeof(T1) >= sizeof(T2), "unsupported operand sizes");
   return (dividend + divisor / 2) / divisor;
}

template <unsigned Size>
inline void trivial_copy(void const * src_v, void * dst_v) {
   auto dst{static_cast<uint8_t *>(dst_v)}, dst_end{dst + Size};
   auto src{static_cast<uint8_t const *>(src_v)};
   while (dst != dst_end) {
      *dst++ = *src++;
   }
}

} //namespace rawr
