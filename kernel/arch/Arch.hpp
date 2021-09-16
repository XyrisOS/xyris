/**
 * @file Arch.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Computer architecture initialization definitions
 * @version 0.3
 * @date 2020-06-01
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

#include <meta/compiler.hpp>
#include <stddef.h>
#include <stdint.h>
#if defined(__i386__)
#    include <arch/i386/Arch.i386.hpp>
#endif

/**
 * @brief Kernel entry point. Performs all kernel initialization
 * and starts the init process(es). Should be called from bootloader
 * entry points.
 *
 * @param info Bootloader information structure
 * @param magic Bootloader magic
 */
extern "C" void kernelEntry(void* info, uint32_t magic);

// Cannot be namespaced since it has to be used by
// functions with C linkage for ASM interoperability
struct registers;

namespace Arch {

struct stackframe;

namespace Memory {


void pagingEnable();
void pagingDisable();
void pageInvalidate(void* pageAddr);
/**
 * @brief Aligns the provided address to the start of its corresponding page address.
 *
 * @param addr Address to be aligned
 * @return uintptr_t Page aligned address value
 */
uintptr_t pageAlign(size_t addr);
/**
 * @brief Check if an address is aligned to a page boundary.
 *
 * @param addr Address to be checked
 * @return true Address is aligned to page boundary
 * @return false Address is not aligned to a page boundary
 */
bool pageIsAligned(size_t addr);

} // !namespace Memory

namespace CPU {

// Architecture initialization
void init();

// Architecture common CPU controls
void interruptsDisable();
void interruptsEnable();

// Critical region lambda function
template<typename Function>
void criticalRegion(Function critWork)
{
    interruptsDisable();
    critWork();
    interruptsEnable();
}

// CPU Identification
const char* vendor();
const char* model();

} // !namespace CPU

// Kernel panic
NORET void panic(const char* msg, const char* file, uint32_t line, const char* func);
NORET void panic(struct registers* regs, const char* file, uint32_t line, const char* func);
#define PANIC(x) Arch::panic((x), __FILE__, __LINE__, __FUNCTION__)

} // !namespace Arch
