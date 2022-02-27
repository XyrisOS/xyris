/**
 * @file Types.i686.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief i686 virtual memory types
 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <stdint.h>
#include <stddef.h>

#define ARCH_PAGE_DIR_ENTRIES       1024
#define ARCH_PAGE_TABLE_ENTRIES     1024
#define ARCH_PAGE_SIZE              4096
#define ARCH_TABLE_SIZE             ARCH_PAGE_SIZE
#define ARCH_DIRECTORY_SIZE         ARCH_PAGE_SIZE
#define ARCH_PAGE_ALIGN             0xFFFFF000
#define ARCH_PAGE_DIR_ENTRY_SHIFT   22          // Shift to convert address to 0-1023 directory index
#define ARCH_PAGE_TABLE_ENTRY_SHIFT 12          // Shift to convert address to page address (2^12 = 4096 = PAGE_SIZE)
#define ARCH_PAGE_TABLE_ENTRY_MASK  0x3ff       // Mask off top 10 bits to get 0-1023 index

#if defined(__cplusplus)
namespace Arch::Memory {
#endif

/**
 * @brief A page is a representation of a virtual address that makes it easier
 * to get the index and offset of a virtual address. Offset is the offset into
 * the page and index is the combined value of the directory and table indexes,
 * which comprise the upper 20 bits of a virtual address.
 *
 * This does not represent an additional layer of indirection but is rather another
 * representation of a virtual address. The *only* time this should be used is when
 * setting the ``pageAddr`` value in a ``TableEntry`` since ``index`` is the physical
 * address of the beginning of the page in physical memory.
 *
 */
struct Page {
    uint32_t offset     : 12; // Offset into the page
    uint32_t pageAddr   : 20; // Physical address of the page
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
    uint32_t writeThrough       : 1;  // Update memory at the same time as cache
    uint32_t cacheDisable       : 1;  // Always read from main memory
    uint32_t accessed           : 1;  // Has the page been accessed since last refresh?
    uint32_t dirty              : 1;  // Has the page been written to since last refresh?
    uint32_t pageAttrTable      : 1;  // Page attribute table (memory cache control)
    uint32_t global             : 1;  // Prevents the TLB from updating the address
    uint32_t unused             : 3;  // Amalgamation of unused and reserved bits
    uint32_t pageAddr           : 20; // Page address (shifted right 12 bits)

#if defined(__cplusplus)
    uintptr_t getPhysicalAddress()
    {
        return pageAddr * ARCH_PAGE_SIZE;
    }
#endif
};

/**
 * @brief Page table structure as defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
struct Table
{
   struct TableEntry entries[ARCH_PAGE_TABLE_ENTRIES]; // All entries for the table

#if defined(__cplusplus)
    struct TableEntry* getTableEntry(size_t idx)
    {
        return &entries[idx];
    }
#endif
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
    uint32_t writeThrough       : 1;  // Update memory at the same time as cache
    uint32_t cacheDisable       : 1;  // Always read from main memory
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
    struct DirectoryEntry entries[ARCH_PAGE_DIR_ENTRIES]; // Pointers that the Intel CPU uses to access pages in memory

#if defined(__cplusplus)
    struct DirectoryEntry* getDirectoryEntry(size_t idx)
    {
        return &entries[idx];
    }
#endif
};

/**
 * @brief Virtual address structure. Represents an address
 * in virtual memory that redirects to a physical page frame.
 * (Chunk of virtual memory)
 */
struct VirtualAddress {
    uint32_t offset      : 12;  // Page offset address
    uint32_t tableIndex  : 10;  // Page table entry
    uint32_t dirIndex    : 10;  // Page directory entry
};

#if defined(__cplusplus)
} // !namespace Arch::Memory
#endif
