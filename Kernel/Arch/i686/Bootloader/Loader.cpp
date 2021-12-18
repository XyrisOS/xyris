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

extern "C"
void stage2Entry(void* info, uint32_t magic)
{
    // TODO: Fix me lol (port boot.s to C)
    kernelEntry((void*)info, magic);
}
