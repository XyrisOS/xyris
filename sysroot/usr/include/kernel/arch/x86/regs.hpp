/**
 * @file regs.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-12-10
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_X86_REGISTERS_HPP
#define PANIX_X86_REGISTERS_HPP

#include <sys/types.hpp>

typedef struct px_register_cr0
{
    uint32_t protected_mode         : 1;        // Protected mode?
    uint32_t monitor_co_processor   : 1;        // Control the interaction of wait instruction?
    uint32_t emulation              : 1;        // Force all FPU operations to be emulated?
    uint32_t task_switched          : 1;        // Save the FPU task context?
    uint32_t extension_type         : 1;        // Is the external math coprocessor an 80287 or 80387?
    uint32_t numeric_error          : 1;        // FPU floating point error reporting?
    uint32_t reserved_a             : 10;       // Reserved
    uint32_t write_protection       : 1;        // Prevent the CPU from writing to read only pages?
    uint32_t reserved_b             : 1;        // Reserved
    uint32_t alignment_mask         : 1;        // Automatic alignment checking?
    uint32_t reserved_c             : 10;       // Reserved
    uint32_t non_write_through      : 1;        // Disable write through caching?
    uint32_t cache_disable          : 1;        // Cache disabled?
    uint32_t paging                 : 1;        // Enable paging?
} px_register_cr0_t;

typedef struct px_register_cr3
{
   uint32_t ignored_a               : 3;        // Ignored
   uint32_t write_through           : 1;        // Page level write through
   uint32_t cache_disable           : 1;        // Cache disable
   uint32_t ignored_b               : 7;        // Ignored
   uint32_t page_dir                : 10;       // Page directory address
} px_register_cr3_t;

#endif /* PANIX_X86_REGISTERS_HPP */