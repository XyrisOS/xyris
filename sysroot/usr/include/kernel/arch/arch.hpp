/**
 * @file arch.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-06-01
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
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

#endif
#if defined(__amd64__) | defined(__x86_64__)
/* Include amd64 (x86_64) headers */

#endif
#if defined(__aarch64__)
/* Include headers for ARM 64 */

#endif /* PANIX_ARCH_HPP */

#endif