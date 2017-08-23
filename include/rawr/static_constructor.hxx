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

/*! Allows to execute code to “construct” a class template that has only static methods, as if actually
instantiating a class with a constructor.

Limitations: a static constructor must leave registers unchanged, or save and restore them as needed.

This works by declare_constructor() terminating its own definition prematurely, then enclosing in an “#if 0”
the declaration of constructor(), so that constructor() itself can re-declare itself in section .init5, which
is just before C++ constructors. This relies on g++ not reordering the two functions during assembly
generation, which seems a valid assumption so far.

The result is that declare_constructor() is generated as an empty function and will just disappear, while the
contents of constructor() are injected directly in .init5, instead of needing a table walked like
__attribute__((constructor)) needs (which takes up almost 40 bytes, and it’s also slower).
*/
#define RAWR_STATIC_CONSTRUCTOR_BEGIN(mangled_qualifier) \
   static __attribute__((naked, used)) void declare_constructor() { \
      __asm__ __volatile__ ( \
         "\n\t" ".size	" mangled_qualifier "19declare_constructorEv, 0" \
         "\n"   ".ifne 0 /* Sorry g++, rawr knows better */" \
      ); \
   } \
   \
   static __attribute__((naked, used)) void constructor() { \
       __asm__ __volatile__ ( \
         "\n"   ".endif" \
         "\n\t" ".section	.init5,\"axG\",@progbits," mangled_qualifier "11constructorEv,comdat" \
         "\n\t" ".global	" mangled_qualifier "11constructorEv" \
         "\n\t" ".type	" mangled_qualifier "11constructorEv, @function" \
         "\n"   mangled_qualifier "11constructorEv:" \
      );

#define RAWR_STATIC_CONSTRUCTOR_END() \
   }
