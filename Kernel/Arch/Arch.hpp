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

#include <Support/compiler.hpp>
#include <stddef.h>
#include <stdint.h>
#if defined(__i686__)
#    include <Arch/i686/Arch.i686.hpp>
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

NORET void haltAndCatchFire();

void registersToString(char* buf, struct registers* regs);
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
