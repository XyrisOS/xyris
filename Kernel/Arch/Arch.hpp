/**
 * @file Arch.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Architecture control and initialization
 * @version 0.3
 * @date 2020-06-01
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#if defined(__i686__)
#    include <Arch/i686/Arch.hpp>
#endif

/**
 * @brief Kernel entry point. Performs all kernel initialization
 * and starts the init process(es). Should be called from bootloader
 * entry points.
 *
 * @param info Bootloader information structure
 * @param magic Bootloader magic
 */
void kernelEntry(void* info, uint32_t magic);

// Cannot be namespaced since it has to be used by
// functions with C linkage for ASM interoperability
struct registers;

namespace Arch {

struct stackframe;

/**
 * @brief Disable interrupts and halts execution
 *
 */
[[noreturn]] [[gnu::always_inline]]
inline void haltAndCatchFire()
{
    while (true) {
        asm volatile ("cli");
        asm volatile ("hlt");
    }
}

/**
 * @brief Write all register names and values as a string to a
 * buffer. Provided buffer size must also take escape and control
 * characters (for formatting) into account.
 *
 * @param buf Buffer to contain register information string
 * @param regs Register structure
 */
void registersToString(char* buf, struct registers* regs);

/**
 * @brief Print register information to all kernel terminals
 * (serial, framebuffer, etc.). Used for panic and debug screens.
 *
 * @param regs Register structure
 */
void registersPrintInformation(struct registers* regs);

}

namespace Arch::CPU {

// Architecture initialization
void init();

// Architecture common CPU controls
void interruptsDisable();
void interruptsEnable();
// TODO: Add interruptsRegisterCallback(uint32_t id, func* cb)

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

} // !namespace Arch::CPU
