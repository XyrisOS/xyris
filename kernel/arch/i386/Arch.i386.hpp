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
#include <arch/i386/regs.hpp>
#include <arch/i386/gdt.hpp>
#include <arch/i386/idt.hpp>
#include <arch/i386/isr.hpp>
#include <arch/i386/panic.hpp>
#include <arch/i386/ports.hpp>
#include <arch/i386/timer.hpp>
#include <cpuid.h>
#include <stddef.h>
#include <stdint.h>

#define ARCH_PAGE_SIZE  0x1000

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

namespace Arch {

struct stackframe {
    struct stackframe* ebp;
    size_t eip;
};

}
