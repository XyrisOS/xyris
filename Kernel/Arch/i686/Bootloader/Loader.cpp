/**
 * @file loader.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2021-12-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <Arch/Arch.hpp>
#include <Arch/i686/Bootloader/Loader.hpp>
#include <Support/sections.hpp>
#include <Panic.hpp>

/* C/C++ runtime initialization & teardown. See crti.s and crtn.s for details
   https://wiki.osdev.org/Calling_Global_Constructors */
extern "C" void _init();
extern "C" void _fini();

/**
 * @brief Check for any required CPU features and panic if
 * any are missing.
 *
 * SSE is disabled with -mno-sse since we don't save SSE/AVX registers
 * for tasks, but we'll likely want to make it a requirement in the future,
 * so we check if SSE and AVX are available.
 *
 */
static void stage2CheckCPUFeatures(void)
{
    __builtin_cpu_init();
    if (!__builtin_cpu_supports("sse"))
    {
        panic("Processor does not have SSE support!");
    }
    if (!__builtin_cpu_supports("avx"))
    {
        panic("Processor does not have AVX support!");
    }
}

extern "C"
void stage2Entry(void* info, uint32_t magic)
{
    // TODO: Fix me lol (port boot.s to C)
    stage2CheckCPUFeatures();

    // Call global constructors
    _init();
    // Enter the high-level kernel
    kernelEntry((void*)info, magic);
    // Call global destructors
    _fini();
    // By this point the kernel should have full execution
    // So, this should never be called unless the kernel returns
    panic("Execution returned to stage2!");
}
