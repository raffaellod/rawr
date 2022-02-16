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

#ifdef RAWR_STARTUP

extern "C" [[noreturn]] __attribute__((section(".init9"))) void uc_main();

__asm__(
   "\n\t" ".section .init0,\"ax\",@progbits"
   "\n\t" ".global __vectors"
   "\n"   "__vectors:"
   "\n\t" "   " RAWR_ASM_JMP " __init"
   "\n\t" ".altmacro"
   "\n\t" ".macro VECTOR_SLOT index"
   "\n\t" "   .weak __vector_\\index"
   /* Too strong; can only work in a separate .o file. Without this line, unexpected IRQs will cause a dirty
   reset (jmp 0x0000). */
#if 0
   "\n\t" "   __vector_\\index = _ZN4rawr5resetEv"
#endif
   "\n\t" "      " RAWR_ASM_JMP " __vector_\\index"
   "\n\t" ".endm"
   "\n\t" ".set __vector_index, 1"
   "\n\t" ".rept " RAWR_TOSTRING(_VECTORS_SIZE / 2 - 1)
   "\n\t" "   VECTOR_SLOT %__vector_index"
   "\n\t" "   .set __vector_index, __vector_index + 1"
   "\n\t" ".endr"
   "\n\t" ".noaltmacro"
   "\n\t"
   "\n\t" ".section .init0,\"ax\",@progbits"
   "\n\t" ".weak	__init"
   "\n"   "__init:"
   "\n"
   "\n\t" ".section .init2,\"ax\",@progbits"
   "\n\t" "   clr     __zero_reg__"
   "\n\t" "   out     __SREG__, r1"
   "\n\t" "   ldi     r28, lo8(" RAWR_TOSTRING(RAMEND) ")"
#ifdef SPH
   "\n\t" "   ldi     r29, hi8(" RAWR_TOSTRING(RAMEND) ")"
   "\n\t" "   out     __SP_H__, r29"
#endif
   "\n\t" "   out     __SP_L__, r28"
   // This can be enabled to handle an accidental return from uc_main().
#if 0
   "\n"
   "\n\t" ".section .fini0,\"ax\",@progbits"
   "\n\t" "   " RAWR_ASM_JMP " _ZN4rawr5resetEv"
#endif
);

__asm__(
   "\n\t" ".section .note.gnu.avr.deviceinfo, \"\", @note"
   "\n\t" "   .long .L__note1_name_end - .L__note1_name_begin"
   "\n\t" "   .long .L__note1_contents_end - .L__note1_contents_begin"
   "\n\t" "   .long 1 /* Note type. */"
   "\n"   ".L__note1_name_begin:"
   "\n\t" "   .asciz \"AVR\""
   "\n\t" "   .balign 4"
   "\n"   ".L__note1_name_end:"
   "\n"   ".L__note1_contents_begin:"
   "\n\t" "   .long 0 /* Flash start address. */"
   "\n\t" "   .long " RAWR_TOSTRING(FLASHEND) " + 1 /* Flash size. */"
   "\n\t" "   .long " RAWR_TOSTRING(RAMSTART) " /* SRAM start address. */"
   "\n\t" "   .long " RAWR_TOSTRING(RAMEND) " - " RAWR_TOSTRING(RAMSTART) " + 1 /* SRAM size. */"
   "\n\t" "   .long 0 /* EEPROM start address. */"
   "\n\t" "   .long " RAWR_TOSTRING(E2END) " + 1 /* EEPROM size. */"
   "\n"   ".L__note1_string_table_begin:"
   "\n\t" "   .long .L__note1_string_table_end - .L__note1_string_table_begin /* Size of string table. */"
   "\n\t" "   .long .L__note1_device_name_begin - .L__note1_string_table_end /* Offset of string [0]. */"
   "\n"   ".L__note1_string_table_end:"
   "\n"   ".L__note1_device_name_begin:"
   "\n\t" "   .asciz \"" RAWR_TOSTRING(__AVR_DEVICE_NAME__) "\""
   "\n"   ".L__note1_contents_end:"
   "\n\t" "   .balign 4"
);

#else //ifdef RAWR_STARTUP

extern "C" [[noreturn]] void uc_main();

extern "C" int __attribute__((weak)) main() {
   uc_main();
}

#endif //ifdef RAWR_STARTUP … else
