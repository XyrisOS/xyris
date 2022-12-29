/**
 * @file Functions.i686.h
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief i686 memory helper functions
 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <Arch/i686/Memory/Types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#if defined(__cplusplus)
namespace Arch::Memory {
#endif

/**
 * @brief Invalidate the page at the given address.
 *
 * @param virtAddr Virtual address of page to be invalidated
 */
static inline void pageInvalidate(void* virtAddr)
{
   asm volatile(
       "invlpg (%0)"
       :
       : "r" (virtAddr)
       : "memory"
    );
}

/**
 * @brief Writes the address of the page directory to CR3. Does not enable paging.
 *
 * @param pageDirPhysAddr Physical address of page directory structure to be used
 */
__attribute__((always_inline))
static inline void setPageDirectory(uintptr_t pageDirPhysAddr)
{
    asm volatile(
        "mov %0, %%cr3"
        :
        : "b" (pageDirPhysAddr)
        : "memory"
    );
}

/**
 * @brief Aligns the provided address to the start of its corresponding page address.
 *
 * @param addr Address to be aligned
 * @return uintptr_t Page aligned address value
 */
static inline uintptr_t pageAlign(uintptr_t addr)
{
    return addr & ARCH_PAGE_ALIGN;
}

/**
 * @brief Aligns the provided address to the start of its next corresponding page address.
 *
 * @param addr Address to be aligned
 * @return uintptr_t Page aligned address value
 */
static inline uintptr_t pageAlignUp(uintptr_t addr)
{
    return (addr + ~ARCH_PAGE_ALIGN) & ARCH_PAGE_ALIGN;
}

/**
 * @brief Check if an address is aligned to a page boundary.
 *
 * @param addr Address to be checked
 * @return true Address is aligned to page boundary
 * @return false Address is not aligned to a page boundary
 */
static inline bool pageIsAligned(uintptr_t addr)
{
    return ((addr % ARCH_PAGE_SIZE) == 0);
}

#if defined(__cplusplus)
} // !namespace Arch::Memory
#endif
