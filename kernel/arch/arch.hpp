/**
 * @file arch.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Computer architecture initialization definitions
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

// Kernel entry point
extern "C" void kernel_main(void* boot_info, uint32_t magic);

// Kernel panic
#define PANIC(x) panic((x), __FILE__, __LINE__, __FUNCTION__)

namespace arch {

// CPU Identification
const char* cpu_get_vendor();
const char* cpu_get_model();

}
