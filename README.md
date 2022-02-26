**RAWR** – Microchip (Atmel) AVR programming framework, the C++ way.

## 1. Introduction

RAWR is a C++ framework for writing AVR programs (“firmware”) in real, modern C++.

C++ on the AVR is no news; some developers out there are already doing it. In most cases though, they end up
with a blend of C++ code and some needed assembly-like C code, because well, some things just need to be done
that way, right?

Wrong.

RAWR abstracts away every need for assembly, bringing to AVR modern C++ features, such as first-class function
objects, an object-oriented hardware abstraction model, user-defined literals, and more.

RAWR makes extensive use of C++ features and g++ tricks to generate as little code a possible, using
compile-time computations or hand-optimized assembly code.

RAWR is distributed as a headers-only library because that allows the C++ compiler to optimize the code most
aggressively; RAWR sources are reguarly tuned and tweaked to generate the smallest amount of code and data,
leaving as much room as possible on your AVR for your projects.


## 2. Getting RAWR

RAWR is [available on GitHub](https://github.com/raffaellod/rawr); to download it, just clone the repository:

```sh
$ git clone https://github.com/raffaellod/rawr.git
$ cd rawr
```

## 3. Installing

RAWR comes as a bunch of header files, so all you need to do to use it is make sure your compiler can find
RAWR’s include directory, i.e. `-Ipath/to/rawr/include` .

TODO: `make install` .


## 4. Using RAWR

RAWR comes with a `make config` utility that will prepare for building the included examples for a specific
AVR MCU connected via avrdude.

After configuring, you can write one of the examples to your MCU via `make EXAMPLE=file_name flash` .

TODO: split makefile into generic include + RAWR-specific top-level, so that other projects may use the
include.

## 5. Compatibility

RAWR is written to rely 100% on avr-libc’s header files for all hardware definitions; this means that as long
as avr-libc supports a given microcontroller, so will RAWR.

With the goal of generating the smallest firmware possible, RAWR by default will not link in avr-libc startup
files, instead providing its own. This behavior can be changed with a one-line change in the prog script.

RAWR requires a g++ version with C++20 support.




--------------------------------------------------------------------------------------------------------------
Copyright 2017, 2022 Raffaello D. Di Napoli

This file is part of RAWR.

RAWR is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

RAWR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
