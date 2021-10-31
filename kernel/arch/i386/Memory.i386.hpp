/**
 * @file Memory.i386.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief i386 memory structures and definitions
 * @version 0.1
 * @date 2021-10-26
 * 
 * @copyright Copyright the Xyris Contributors (c) 2021
 * 
 */
#pragma once

#define ARCH_PAGE_ALIGN 0xFFFFF000
#define ARCH_PAGE_SIZE  0x1000

namespace Arch::Memory {

/**
 * @brief Page frame structure. This represents a the
 * address to a single unit of memory in RAM.
 * (Chunk of physical memory)
 *
 */
struct Frame {
    uint32_t offset : 12; // Page offset address
    uint32_t index  : 20; // Page frame index
};

/**
 * @brief Virtual address structure. Represents an address
 * in virtual memory that redirects to a physical page frame.
 * (Chunk of virtual memory)
 */
struct Page {
    uint32_t offset      : 12;  // Page offset address
    uint32_t tableIndex  : 10;  // Page table entry
    uint32_t dirIndex    : 10;  // Page directory entry
};

/**
 * @brief Address union. Represents a memory address that can
 * be used to address a page in virtual memory, a frame in
 * physical memory. Allows setting the value of these addresses
 * by also including an uintptr_t representation.
 *
 */
union Address {
    struct Page page;
    struct Frame frame;
    uintptr_t val;
};

/**
 * @brief Page table entry defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
struct TableEntry
{
    uint32_t present            : 1;  // Page present in memory
    uint32_t readWrite          : 1;  // Read-only if clear, readwrite if set
    uint32_t usermode           : 1;  // Supervisor level only if clear
    uint32_t writeThrough       : 1;  // Page level write through
    uint32_t cacheDisable       : 1;  // Disables TLB caching of page entry
    uint32_t accessed           : 1;  // Has the page been accessed since last refresh?
    uint32_t dirty              : 1;  // Has the page been written to since last refresh?
    uint32_t pageAttrTable      : 1;  // Page attribute table (memory cache control)
    uint32_t global             : 1;  // Prevents the TLB from updating the address
    uint32_t unused             : 3;  // Amalgamation of unused and reserved bits
    uint32_t frame              : 20; // Frame address (shifted right 12 bits)
};

/**
 * @brief Page table structure as defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
struct Table
{
   struct TableEntry pages[1024]; // All entries for the table
};

/**
 * @brief Page directory entry structure as defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
struct DirectoryEntry
{
    uint32_t present            : 1;  // Is the page present in physical memory?
    uint32_t readWrite          : 1;  // Is the page read/write or read-only?
    uint32_t usermode           : 1;  // Can the page be accessed in usermode?
    uint32_t writeThrough       : 1;  // Is write-through cache enabled?
    uint32_t cacheDisable       : 1;  // Can the page be cached?
    uint32_t accessed           : 1;  // Has the page been accessed?
    uint32_t ignoredA           : 1;  // Ignored
    uint32_t size               : 1;  // Is the page 4 Mb (enabled) or 4 Kb (disabled)?
    uint32_t ignoredB           : 4;  // Ignored
    uint32_t tableAddr          : 20; // Physical address of the table
};

/**
 * @brief Page directory contains pointers to all of the virtual memory addresses for the
 * page tables along with their corresponding physical memory locations of the page tables.
 * Page table entry defined in accordance to the Intel Developer Manual Vol. 3a p. 4-12.
 *
 */
struct Directory
{
    struct DirectoryEntry entries[1024];     // Pointers that the Intel CPU uses to access pages in memory
};

/**
 * @brief Invalidate the page at the given address. Implementations are architecture
 * specific.
 *
 * @param addr Address of page to be invalidated
 */
inline void pageInvalidate(void* addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

/**
 * @brief Aligns the provided address to the start of its corresponding page address.
 *
 * @param addr Address to be aligned
 * @return uintptr_t Page aligned address value
 */
inline uintptr_t pageAlign(size_t addr)
{
    return addr & ARCH_PAGE_ALIGN;
}

/**
 * @brief Check if an address is aligned to a page boundary.
 *
 * @param addr Address to be checked
 * @return true Address is aligned to page boundary
 * @return false Address is not aligned to a page boundary
 */
inline bool pageIsAligned(size_t addr)
{
    return ((addr % ARCH_PAGE_SIZE) == 0);
}

}
