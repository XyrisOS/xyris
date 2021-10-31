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

#include <stdint.h>
#include <stddef.h>
#if defined(__i386__)
#    include <arch/i386/Memory.i386.hpp>
#endif

#define ADDRESS_SPACE_SIZE  0x100000000
// TODO: Add this to memory class (#326)
#define ADDRESS_TO_PAGE_IDX(addr) ((addr) / ARCH_PAGE_SIZE)
#define MEM_BITMAP_SIZE ((ADDRESS_SPACE_SIZE / ARCH_PAGE_SIZE) / (sizeof(size_t) * CHAR_BIT))

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
