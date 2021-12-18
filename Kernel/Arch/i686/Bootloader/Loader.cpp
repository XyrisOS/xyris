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
    // zero the kernel BSS
    for (size_t i = 0; i < _BSS_SIZE; i++) {
        ((uint8_t*)_BSS_START)[i] = 0;
    }

    // TODO: Fix me lol (port boot.s to C)
    stage2CheckCPUFeatures();
    kernelEntry((void*)info, magic);
    panic("Execution returned to stage2!");
}
