/**
 * @file Arch.i386.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief i386 architecture implementation of Arch.hpp
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <arch/i386/gdt.hpp>
#include <arch/i386/idt.hpp>
#include <arch/i386/isr.hpp>
#include <arch/i386/panic.hpp>
#include <arch/i386/ports.hpp>
#include <arch/i386/timer.hpp>
#include <cpuid.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief x86 BIOS based VGA pointers and data.
 * Used for printing to the VGA screen via BIOS memory.
 */
#define X86_TTY_WIDTH 80
#define X86_TTY_HEIGHT 25
#define X86_IND_X 79
#define X86_IND_Y 0
inline uint16_t* x86_bios_vga_mem = (uint16_t*)0x000B8000;

// List of all exceptions and their associated english descriptions
extern const char* exception_descriptions[32][16];

/*
 *    _          _      _____
 *   /_\  _ _ __| |_   |_   _|  _ _ __  ___ ___
 *  / _ \| '_/ _| ' \    | || || | '_ \/ -_|_-<
 * /_/ \_\_| \__|_||_|   |_| \_, | .__/\___/__/
 *                           |__/|_|
 */

/**
 * @brief A structure definining values for every since x86 register.
 * Used when in various x86 architecture functions and panic.
 */
struct registers {
    uint32_t ds;                                         /* Data segment selector */
    uint32_t edi, esi, ebp, ignored, ebx, edx, ecx, eax; /* Pushed by pusha. */
    uint32_t int_num, err_code;                          /* Interrupt number and error code (if applicable) */
    uint32_t eip, cs, eflags, esp, ss;                   /* Pushed by the processor automatically */
};

struct stackframe {
    struct stackframe* ebp;
    size_t eip;
};
