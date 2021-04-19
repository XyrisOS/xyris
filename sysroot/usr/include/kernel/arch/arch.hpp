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

// Kernel entry point
extern "C" void px_kernel_main(void* boot_info, uint32_t magic);
void px_arch_init();

/* Shared i386 & amd64 */
#if defined(__i386__) | defined(__x86_64__)
#include <cpuid.h>

// Externed architecture variables
extern const char* px_exception_descriptions[];
// Architecture typedefs
typedef void (*isr_t)(registers_t *);
typedef struct stivale2_struct stivale2_info;
// Architecture inline functions
static inline void px_arch_cpuid(int flag, unsigned long eax, unsigned long ebx, unsigned long ecx, unsigned long edx)
{
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(flag));
}
static inline int px_arch_cpuid(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+1)), "=c"(*(regs+2)), "=d"(*(regs+3)) : "a"(flag));
    return (int)regs[0];
}
// i386+ & amd64 functions
const char* px_cpu_get_vendor();
const char* px_cpu_get_model();

#endif /* End shared i386 & amd64 */

/* i386+ Specific Code */
#if defined(__i386__)
/* Include x86 (i386) headers */
#include <arch/i386/i386.hpp>

#endif /* End x86 specific code*/

