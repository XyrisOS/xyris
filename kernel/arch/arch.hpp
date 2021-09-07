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
#include <meta/compiler.hpp>
#if defined(__i386__)
#    include <arch/i386/arch-i386.hpp>
#endif

// Architecture types (forward declarations)
struct registers;

// Kernel panic
#define PANIC(x) panic((x), __FILE__, __LINE__, __FUNCTION__)

/**
 * @brief Halts kernel execution and prints provided info.
 *
 * @param msg Explaination of what happened
 * @param file File causing the issue
 * @param line Line with the error
 * @param func Function containing error
 */
NORET void panic(const char* msg, const char* file, uint32_t line, const char* func);

/**
 * @brief Halts kernel execution and prints register info.
 *
 * @param regs Registers struct
 * @param file File causing the issue
 * @param line Line with the error
 * @param func Function containing error
 */
NORET void panic(struct registers* regs, const char* file, uint32_t line, const char* func);

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

// Architecture common memory controls
void pagingEnable();
void pagingDisable();
void pagingInvalidate(void* pageAddr);

// CPU Identification
const char* cpuGetVendor();
const char* cpuGetModel();

} // !namespace Arch
