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

#include <sys/panix.hpp>

#ifndef PANIX_ARCH_HPP
#define PANIX_ARCH_HPP

const char* const px_cpu_get_vendor();
const char* const px_cpu_get_model();

struct registers;
typedef struct registers registers_t;
typedef void (*isr_t)(registers_t *);

#if defined(__i386__) | defined(__i686__)
/* Include i386 (x86) headers */
/* The primary target for Panix is Intel i686 but GCC
   seems to count i386 as in the same group. Threw them
   both in just to be safe. */
#include <arch/x86/gdt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/isr.hpp>
#include <arch/x86/timer.hpp>
#include <arch/x86/ports.hpp>

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

/**
 * @brief x86 BIOS based VGA pointers and data.
 * Used for printing to the VGA screen via BIOS memory.
 */
#define X86_TTY_WIDTH   80
#define X86_TTY_HEIGHT  25
#define X86_IND_X       79
#define X86_IND_Y       0
inline uint16_t* x86_bios_vga_mem = (uint16_t*) 0x000B8000;

#endif/* x86 */
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
} registers_t;

#endif /* x86_64 */
#if defined(__arm__)
/* Include headers for ARM 32 */

#endif /* arm */
#if defined(__aarch64__)
/* Include headers for ARM 64 */

#endif /* arm64 */
#endif /* PANIX_ARCH_HPP */