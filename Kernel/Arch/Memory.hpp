/**
 * @file Memory.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Architecture memory management & paging API
 * @version 0.1
 * @date 2021-09-18
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <stddef.h>
#include <stdint.h>
#if defined(__i686__)
#    include <Arch/i686/Memory/Address.hpp>
#    include <Arch/i686/Memory/Functions.h>
#    include <Arch/i686/Memory/Types.h>
#endif

#define B_TO_KB(b) ((b) / 1024)
#define KB_TO_MB(kb) ((kb) / 1024)
#define MB_TO_GB(mb) ((mb) / 1024)

#define B_TO_MB(b) KB_TO_MB(B_TO_KB(b))
#define B_TO_GB(b) MB_TO_GB(B_TO_MB(b))

#define B_TO_PAGES(bytes) (((bytes) + ARCH_PAGE_SIZE - 1) / ARCH_PAGE_SIZE)
#define PAGE_COUNT(s) ((s) / ARCH_PAGE_SIZE) + 1;

#define ADDRESS_SPACE_SIZE 0x100000000
// TODO: Add this to memory class (#326)
#define PAGE_IDX_TO_ADDRESS(idx) ((idx) * ARCH_PAGE_SIZE)
#define ADDRESS_TO_PAGE_IDX(addr) ((addr) / ARCH_PAGE_SIZE)
#define MEM_BITMAP_SIZE (ADDRESS_SPACE_SIZE / ARCH_PAGE_SIZE)

namespace Arch::Memory {

/**
 * @brief Enable hardware paging
 *
 */
void pagingEnable();

/**
 * @brief Disable hardware paging
 *
 */
void pagingDisable();

} // !namespace Arch::Memory
