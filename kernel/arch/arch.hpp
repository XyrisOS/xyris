/**
 * @file arch.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-06-01
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#if defined(__i386__)
#    include <arch/i386/i386.hpp>
#endif

struct registers;
typedef struct registers registers_t;

// Externed architecture variables
extern const char* exception_descriptions[];
// Architecture typedefs
typedef struct stivale2_struct stivale2_info;
// Kernel entry point
extern "C" void kernel_main(void* boot_info, uint32_t magic);
// i386+ & amd64 functions
const char* cpu_get_vendor();
const char* cpu_get_model();
void parse_multiboot2(void* info);
void parse_stivale2(void* info);

/**
 * @brief x86 BIOS based VGA pointers and data.
 * Used for printing to the VGA screen via BIOS memory.
 */
#define X86_TTY_WIDTH 80
#define X86_TTY_HEIGHT 25
#define X86_IND_X 79
#define X86_IND_Y 0
inline uint16_t* x86_bios_vga_mem = (uint16_t*)0x000B8000;

/**
 * @brief A structure definining values for every since x86 register.
 * Used when in various x86 architecture functions and panic.
 */
typedef struct registers {
    uint32_t ds;                                         /* Data segment selector */
    uint32_t edi, esi, ebp, ignored, ebx, edx, ecx, eax; /* Pushed by pusha. */
    uint32_t int_num, err_code;                          /* Interrupt number and error code (if applicable) */
    uint32_t eip, cs, eflags, esp, ss;                   /* Pushed by the processor automatically */
} registers_t;

struct stackframe {
    struct stackframe* ebp;
    size_t eip;
};
