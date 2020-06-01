/**
 * @file arch.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2020-06-01
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2020
 * 
 */

#include <sys/sys.hpp>
#include <gnu/cpuid.hpp>

const char* const px_cpu_get_vendor();

#if defined(__i386__) | defined(__i686__)
/* Include i386 (x86) headers */
/* The primary target for Panix is Intel i686 but GCC 
   seems to count i386 as in the same group. Threw them
   both in just to be safe. */
#include <arch/x86/gdt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/isr.hpp>
#include <arch/x86/timer.hpp>

#endif
#if defined(__amd64__) | defined(__x86_64__)
/* Include amd64 (x86_64) headers */

#endif
#if defined(__aarch64__)
/* Include headers for ARM 64 */

#endif