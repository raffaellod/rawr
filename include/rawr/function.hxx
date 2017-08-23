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

#include <rawr/abort.hxx>
#include <rawr/misc.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rawr {

// Look-alike of std::function, with support for binding to “this” but not to arguments.
template <typename F>
class function;

template <typename Ret, typename... Args>
class function<Ret (Args...)> {
private:
   typedef Ret (* nonmember_target_fn)(Args...);
   template <typename T>
   using member_target_fn = Ret (T::*)(Args...);

   union target_t {
      nonmember_target_fn func;
      void * obj;

      constexpr target_t() :
         obj(nullptr) {
      }

      constexpr target_t(nonmember_target_fn func_) :
         func(func_) {
      }

      constexpr target_t(void * obj_) :
         obj(obj_) {
      }
   };

   static Ret nonmember_caller_impl(target_t const * target_, Args... args) {
      return (*target_->func)(args...);
   }

   template <typename T, member_target_fn<T> M>
   struct this_caller_impl {
      static Ret call(target_t const * target_, Args... args) {
         return (static_cast<T *>(target_->obj)->*M)(args...);
      }
   };

public:
   template <typename T, member_target_fn<T> M>
   struct bind_stage2 {
      T * obj;
   };

   template <typename T>
   struct bind_stage1 {
      T * obj;

      template <member_target_fn<T> M>
      constexpr function to() {
         return bind_stage2<T, M>{obj};
      }
   };

public:
   constexpr function() :
      caller(nullptr) {
   }

   /* target_ is T instead of Ret (*)(Args...) so it can match a capture-less lambda, which can then be
   implicitly converted to Ret (*)(Args...). */
   template <typename T>
   /*implicit*/ constexpr function(T target_) :
      caller(&nonmember_caller_impl),
      target(target_) {
   }

   constexpr function(function const & src) :
      caller(src.caller),
      target(src.target) {
   }

   function & operator=(decltype(nullptr)) {
      caller = nullptr;
      return *this;
   }

   function & operator=(function const & src) {
      caller = src.caller;
      target = src.target;
      return *this;
   }

   explicit constexpr operator bool() const {
      return caller != nullptr;
   }

   constexpr bool operator==(decltype(nullptr)) const {
      return caller == nullptr;
   }

   constexpr bool operator!=(decltype(nullptr)) const {
      return !operator==(nullptr);
   }

   Ret operator()(Args... args) const {
      if (!caller) {
         abort();
      }
      return caller(&target, args...);
   }

   template <typename T>
   static constexpr bind_stage1<T> bind(T * target_) {
      return bind_stage1<T>{target_};
   }

private:
   template <typename T, member_target_fn<T> M>
   explicit constexpr function(bind_stage2<T, M> bound) :
      caller(&this_caller_impl<T, M>::call),
      target(bound.obj) {
   }

private:
   Ret (* caller)(target_t const *, Args...);
   target_t target;
};

} //namespace rawr
