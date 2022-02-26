# -*- coding: utf-8; mode: Makefile; tab-width: 3 -*-
#
# Copyright 2017, 2022 Raffaello D. Di Napoli
#
# This file is part of RAWR.
#
# RAWR is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
# Lesser General Public License as published by the Free Software Foundation.
#
# RAWR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
# for more details.
#-------------------------------------------------------------------------------------------------------------

# Note: this makefile uses a few GNU make-specific syntax extensions, hence the file name; however it is a
# future goal for this file to include conditionals to cover more make vendors, so vendor-specific extensions
# should be avoided where possible and not too inconvenient.

.POSIX:
.SUFFIXES:

.PHONY: help
help:
	@echo 'Example invocations:'
	@echo
	@echo 'make config'
	@echo '  Generates a Makefile.config file with the MCU model to compile for, and its operating frequency.'
	@echo 'make all'
	@echo '  Builds all sources in examples/file_name.cxx into hex/examples/file_name.hex .'
	@echo 'make EXAMPLE=file_name'
	@echo '  Like above, but only build examples/file_name.cxx .'
	@echo 'make EXAMPLE=file_name flash'
	@echo '  Like above, then write the generated hex file to a connected AVR MCU via avrdude.'
	@echo 'make MCU=ATmega328p F_CPU=5000 EXAMPLE=blink flash'
	@echo '  Like above, but overrides MCU model and F_CPU on the fly via command line.'
	@echo 'make HEX=path/to/file_name.hex flash'
	@echo '  Instead of writing one of the examples, write the hex file indicated by path.'
	@echo 'make clean'
	@echo '  Delete all build output files.'
	@echo 'make distclean'
	@echo '  Like above, but also delete Makefile.config .'

# Configuration ----------------------------------------------------------------------------------------------

# $(O) is where we’ll put all generated files.
ifneq ($(O),)
   # Ensure $(O) ends with a backslash.
   O:=$(O:%/=%)/
endif

ifdef USE_CLANG
   AVR_CXX?=clang++ --target=avr
else
   AVR_CXX?=avr-g++
endif
AVR_CPP?=$(AVR_CXX) -E
AVRDUDE?=avrdude
HOST_CXX?=g++

# Save this for now, so we can use += while adding to it, without overriding command-line overrides.
CUSTOM_AVR_CXXFLAGS:=$(AVR_CXXFLAGS)
AVR_CXXFLAGS:=

# Select language/optimizations/debugging options.
AVR_CXXFLAGS+= -std=c++20
AVR_CXXFLAGS+= -g0 -Os
AVR_CXXFLAGS+= -fvisibility=internal
ifndef USE_CLANG
   AVR_CXXFLAGS+= -fno-fat-lto-objects
endif

# RAWR-specific flags.
AVR_CXXFLAGS+= -Iinclude
# Change to false to link against avr-libc’s startup files for maximum compatibility.
# Note however that doing so will kill soma/all rawr interrupt vectors due to the hacky way we declare them,
# and will also result in slightly larger code size.
ifneq ($(RAWR_STARTUP),0)
   AVR_CXXFLAGS+= -DRAWR_STARTUP -fno-toplevel-reorder
   ifdef USE_CLANG
      AVR_LDFLAGS+= -nostdlib
   else
      AVR_LDFLAGS+= -nostartfiles
   endif
endif

# Enable more warnings.
WARN_CXXFLAGS:=-Wall
# Enable extra warnings not enabled by -Wall.
WARN_CXXFLAGS+= -Wextra
# Issue all the warnings demanded by strict ISO C++.
WARN_CXXFLAGS+= -pedantic
# Warn for implicit conversions that may alter a value.
WARN_CXXFLAGS+= -Wconversion
ifndef USE_CLANG
   # Warn about suspicious uses of logical operators in expressions.
   WARN_CXXFLAGS+= -Wlogical-op
endif
# Warn if a global function is defined without a previous declaration.
WARN_CXXFLAGS+= -Wmissing-declarations
# Warn if a struct has “packed” attribute but that has no effect on its layout or size.
WARN_CXXFLAGS+= -Wpacked
# Warn when a local symbol shadows another symbol.
WARN_CXXFLAGS+= -Wshadow
# Warn for implicit conversions that may change the sign of an integer value.
WARN_CXXFLAGS+= -Wsign-conversion
# Warn if an undefined identifier is evaluated in “#if”.
WARN_CXXFLAGS+= -Wundef

# Project-specific rules -------------------------------------------------------------------------------------

.PHONY: all
ifneq ($(EXAMPLE),)
ifeq ($(HEX),)
# Set HEX in case flash is one of the targets.
HEX:=$(O)hex/examples/$(EXAMPLE).hex
endif
# In any case, reduce all to just the requested example.
all: $(O)bin/examples/$(EXAMPLE).bin
endif

define single_cxx_file_bin_rule
ifeq ($(EXAMPLE),)
# Without a specific example being requested, build them all.
all: $$(O)bin/$(cxx_file:.cxx=).bin
endif
$$(O)bin/$(cxx_file:.cxx=).bin: $$(O)int/$(cxx_file).o
endef

$(foreach cxx_file,$(shell find examples -type f -name '*.cxx'),$(eval $(call single_cxx_file_bin_rule)))

# Dynamic configuration rules --------------------------------------------------------------------------------

.PHONY: config
config: $(O)Makefile.config

$(O)Makefile.config:
ifneq ($(O),)
	mkdir -p $(O)
endif
	AVRDUDE="$(AVRDUDE)" ./mkconfig >$(O)Makefile.config

-include $(O)Makefile.config

AVR_CXXFLAGS+= -mmcu=$(shell echo $(MCU) | tr '[[:upper:]]' '[[:lower:]]') -DF_CPU=$(F_CPU)
# Now we can pull this back in in the correct order for it to override anything.
AVR_CXXFLAGS+= $(CUSTOM_AVR_CXXFLAGS)

# Generic build/write rules ----------------------------------------------------------------------------------

$(O)asm/%.cxx.a: %.cxx
	mkdir -p $$(dirname $@) && \
	$(AVR_CXX) $(AVR_CXXFLAGS) -S -o $@ $<

$(O)int/%.cxx.o: %.cxx
	mkdir -p $$(dirname $@) && \
	$(AVR_CXX) $(AVR_CXXFLAGS) -c -o $@ $<

$(O)bin/%.bin:
	mkdir -p $$(dirname $@) && \
	$(AVR_CXX) $(AVR_CXXFLAGS) $(AVR_LDFLAGS) -o $@ $<

$(O)asm/%.bin.a: $(O)bin/%.bin
	mkdir -p $$(dirname $@) && \
	avr-objdump -CS $< >$@

$(O)hex/%.hex: $(O)bin/%.bin
	mkdir -p $$(dirname $@) && \
	avr-objcopy -j .text -j .data -O ihex $< $@ && \
	echo "  Flash size: $$(( \
		$$(sed <$@ -ne 's/^:\([A-fa-f0-9]\{2\}\)[A-fa-f0-9]\{4\}00.*$$/+0x\1/p') \
	)) bytes"

#! Programs a connected MCU with the .hex file specified via `make HEX=...` .
.PHONY: flash
ifneq ($(HEX),)
flash: $(HEX)
	while ! $(AVRDUDE) -p $(MCU) -U flash:w:$(HEX); do \
	   echo "avrdude failed ($${?}); trying again"; \
	   sleep 0.25; \
	done
else
flash:
	@echo 'Please provide HEX=... on make command line to specify which .hex file to write' >&2 && exit 1
endif

# Generic maintenance rules ----------------------------------------------------------------------------------

#! Deletes all files from the current directory that are normally created by building the program.
.PHONY: clean
clean:
	rm -rf $(O)asm $(O)bin $(O)hex $(O)int

#! Deletes all files from the current directory that are created by configuring or building the program.
.PHONY: distclean
distclean: clean
	rm -f $(O)Makefile.config
