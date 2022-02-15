/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2017, 2022 Raffaello D. Di Napoli

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

namespace rawr {

#ifndef RAWR_FUNCTION_DEFAULT_MAX_LAMBDA_SIZE
   // This may be overridden, at the cost of enlarging sizeof(rawr::function) for the entire binary.
   #define RAWR_FUNCTION_DEFAULT_MAX_LAMBDA_SIZE 6
#endif

// Equivalent to std::function.
template <typename F>
class function;

template <typename Ret, typename... Args>
class function<Ret (Args...)> {
private:
   struct impl {
      /* Note the lack of a virtual destructor: using one pulls in operator delete(void *, size_t), which we
      don’t want to have to declare because there’s no implementation.
      Instead, we have a destruct() virtual method that is called before the object is considered gone.
      Aside from avoiding the annoying warnings about operator delete needing overloads for completeness’
      sake, this approach also saves TWO Flash bytes.

      For the record, these alternative approaches were attempted as well (building the blink example as a
      reference):
       • no virtuals, using a struct to mimic the vtable you see here: that ended up costing 20 more Flash
         bytes;
       • no virtuals, using a static function to mimic the vtable you see here: while that avoided pulling in
         the __do_copy_data function, it ended up costing 90 more Flash bytes than this approach.

      It’s nice knowing that using a compiler-provided facility results in the smallest code size. */

      virtual Ret call(Args...) const = 0;

      virtual void copy(void * dst_storage) const {
         trivial_copy<sizeof impl_storage>(this, dst_storage);
      }

      virtual void destruct() {
         // No-op.
      }

      virtual void move(void * dst_storage) {
         trivial_copy<sizeof impl_storage>(this, dst_storage);
      }
   };

   template <typename L>
   struct lambda_impl : impl {
      explicit lambda_impl(L lambda_) :
         lambda{forward<L>(lambda_)} {
      }

      virtual Ret call(Args... args) const override {
         return lambda(args...);
      }

      virtual void copy(void * dst_storage) const override {
         if (__is_trivially_constructible(L, L const &)) {
            impl::copy(dst_storage);
         } else {
            new(dst_storage) lambda_impl{lambda};
         }
      }

      virtual void destruct() override {
         if (__has_trivial_destructor(L)) {
            impl::destruct();
         } else {
            lambda.~L();
         }
      }

      virtual void move(void * dst_storage) override {
         if (__is_trivially_constructible(L, L &&)) {
            impl::move(dst_storage);
         } else {
            new(dst_storage) lambda_impl{rawr::move(lambda)};
         }
      }

      L lambda;
   };

public:
   constexpr function() :
      set{false} {
   }

   function(function && src) :
      set{src.set} {
      if (set) {
         src.impl()->move(impl_storage);
         src.impl()->destruct();
         src.set = false;
      }
   }

   function(function const & src) :
      set{src.set} {
      if (set) {
         src.impl()->copy(impl_storage);
      }
   }

   template <typename L>
   /*implicit*/ constexpr function(L lambda) :
      set{true} {
      static_assert(sizeof(lambda_impl<L>) <= sizeof impl_storage, "cannot use lambdas of size > sizeof impl_storage - vtable cost");
      new(impl_storage) lambda_impl<L>{forward<L>(lambda)};
   }

   ~function() {
      if (set) {
         impl()->destruct();
      }
   }

   function & operator=(function && src) {
      if (&src != this) {
         if (set) {
            impl()->destruct();
         }
         set = src.set;
         if (set) {
            src.impl()->move(impl_storage);
            src.impl()->destruct();
            src.set = false;
         }
      }
      return *this;
   }

   function & operator=(function const & src) {
      if (&src != this) {
         if (set) {
            impl()->destruct();
         }
         set = src.set;
         if (set) {
            src.impl()->copy(impl_storage);
         }
      }
      return *this;
   }

   function & operator=(decltype(nullptr)) {
      if (set) {
         impl()->destruct();
         set = false;
      }
      return *this;
   }

   explicit constexpr operator bool() const {
      return set;
   }

   constexpr bool operator==(decltype(nullptr)) const {
      return !set;
   }

   constexpr bool operator!=(decltype(nullptr)) const {
      return set;
   }

   Ret operator()(Args... args) const {
      return impl()->call(args...);
   }

private:
   struct impl * impl() {
      return static_cast<struct impl *>(static_cast<void *>(impl_storage));
   }

   struct impl const * impl() const {
      return static_cast<struct impl const *>(static_cast<void const *>(impl_storage));
   }

private:
   bool set;
   uint8_t impl_storage[RAWR_FUNCTION_DEFAULT_MAX_LAMBDA_SIZE];
};

/*template <typename L>
inline auto make_function(L lambda) {
   return function<decltype(&L::operator())>(lambda);
}*/

} //namespace rawr
