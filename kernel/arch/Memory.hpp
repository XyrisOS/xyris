/**
 * @file Memory.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Architecture memory management API
 * @version 0.1
 * @date 2021-09-18
 * 
 * @copyright Copyright the Xyris Contributors (c) 2021
 * 
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

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
/**
 * @brief Invalidate the page at the given address. Implementations are architecture
 * specific.
 *
 * @param addr Address of page to be invalidated
 */
void pageInvalidate(void* addr);
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

} // !namespace Arch::Memory
