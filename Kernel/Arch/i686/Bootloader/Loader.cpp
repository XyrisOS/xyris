/**
 * @file loader.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Bootloader agnostic pre-kernel initialization
 * @version 0.1
 * @date 2021-12-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <Arch/Arch.hpp>
#include <Arch/i686/Bootloader/Loader.hpp>
#include <Panic.hpp>
#include <Support/sections.hpp>

/* C/C++ runtime initialization & teardown. See crti.s and crtn.s for details
   https://wiki.osdev.org/Calling_Global_Constructors */
extern "C" void _init();
extern "C" void _fini();

/**
 * @brief Bootloader agnostic pre-kernel entry initialization. Stage2 should not
 * access any bootloader information and should only perform bootloader agnostic
 * tasks before entering the kernel proper.
 *
 */
extern "C" void stage2Entry(void* info, uint32_t magic)
{
    // Call global constructors
    _init();
    // Enter the high-level kernel
    kernelEntry(info, magic);
    // Call global destructors
    _fini();
    // By this point the kernel should have full execution
    // So, this should never be called unless the kernel returns
    panic("Execution returned to stage2!");
}
