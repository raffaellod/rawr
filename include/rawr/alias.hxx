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

#define RAWR_ALIAS(mangled_alias, mangled_original) \
   __asm__ __volatile__ ( \
      "\n\t" ".weak " mangled_alias \
      "\n"   mangled_alias " = " mangled_original \
   )
