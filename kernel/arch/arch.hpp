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
#include <boot/Handoff.hpp>
#if defined(__i386__)
#    include <arch/i386/i386.hpp>
#endif

// Kernel entry point
extern "C" void kernel_main(void* boot_info, uint32_t magic);

// Kernel panic
#define PANIC(x) panic((x), __FILE__, __LINE__, __FUNCTION__)

namespace Arch {

// Architecture initialization
void cpuInit();

// Architecture common CPU controls
void interruptsDisable();
void interruptsEnable();
// Critical region lambda function
template <typename Function>
void criticalRegion(Function critWork)
{
    interruptsDisable();
    critWork();
    interruptsEnable();
}

// CPU Identification
const char* cpuGetVendor();
const char* cpuGetModel();

} // !namespace Arch
