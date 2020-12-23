/**
 * @file arch.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-06-01
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

struct registers;
typedef struct registers registers_t;

/* Shared i386 & amd64 */
#if defined(__i386__) | defined(__i686__) | defined(__amd64__) | defined(__x86_64__)
// Externed architecture variables
extern const char* exception_descriptions[];
// Architecture typedefs
typedef void (*isr_t)(registers_t *);
typedef struct stivale2_struct stivale2_info;
// Architecture inline functions
static inline void arch_cpuid(int flag, unsigned long eax, unsigned long ebx, unsigned long ecx, unsigned long edx)
{
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(flag));
}
static inline int arch_cpuid(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+1)), "=c"(*(regs+2)), "=d"(*(regs+3)) : "a"(flag));
    return (int)regs[0];
}
// Kernel entry point
extern "C" void kernel_main(void* boot_info, uint32_t magic);
// i386+ & amd64 functions
const char* cpu_get_vendor();
const char* cpu_get_model();
void parse_multiboot2(void* info);
void parse_stivale2(void* info);
// Shared library code for i386+ & amd64 family
#include <cpuid.h>
#include <arch/i386/gdt.hpp>
#include <arch/i386/tss.hpp>
#include <arch/i386/idt.hpp>
#include <arch/i386/isr.hpp>
#include <arch/i386/timer.hpp>
#include <arch/i386/ports.hpp>

/**
 * @brief x86 BIOS based VGA pointers and data.
 * Used for printing to the VGA screen via BIOS memory.
 */
#define X86_TTY_WIDTH   80
#define X86_TTY_HEIGHT  25
#define X86_IND_X       79
#define X86_IND_Y       0
inline uint16_t* x86_bios_vga_mem = (uint16_t*) 0x000B8000;

#endif /* End shared i386 & amd64 */

/* i386+ Specific Code */
#if defined(__i386__) | defined(__i686__)
/* Include x86 (i386) headers */
/**
 * @brief A structure definining values for every since x86 register.
 * Used when in various x86 architecture functions and panic.
 */
typedef struct registers {
    uint32_t ds;                                          /* Data segment selector */
    uint32_t edi, esi, ebp, ignored, ebx, edx, ecx, eax;  /* Pushed by pusha. */
    uint32_t int_num, err_code;                           /* Interrupt number and error code (if applicable) */
    uint32_t eip, cs, eflags, esp, ss;                    /* Pushed by the processor automatically */
} registers_t;

struct stackframe {
  struct stackframe* ebp;
  size_t eip;
};

#endif /* End x86 specific code*/

/* AMD64 Specific Code */
#if defined(__amd64__) | defined(__x86_64__)
/* Include amd64 (x86_64) headers */
/**
 * @brief A structure definining values for every since x86_64 register.
 * Used when in various x86_64 architecture functions and panic.
 */
typedef struct registers {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp;    /* General purpose registers */
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;      /* General purpose registers */
    uint64_t rip, cs, ds, ss, es, fs, gs;               /* Pointer and segment registers */
    uint64_t rflags, cr0, cr2, cr3, cr4, cr8;           /* Flags and control registers */
    uint64_t int_num, err_code;                         /* Interrupt number and error code (if applicable) */
} registers_t;

#endif /* End x86_64 specific code */

#if defined(__arm__)
/* Include headers for ARM 32 */

#endif /* arm */

#if defined(__aarch64__)
/* Include headers for ARM 64 */

#endif /* arm64 */
