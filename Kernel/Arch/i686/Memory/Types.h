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
#define ARCH_PAGE_ALIGN             0xFFFFF000
#define ARCH_PAGE_DIR_ENTRY_SHIFT   22          // Shift to convert address to 0-1023 directory index
#define ARCH_PAGE_TABLE_ENTRY_SHIFT 12          // Shift to convert address to page address (2^12 = 4096 = PAGE_SIZE)
#define ARCH_PAGE_TABLE_ENTRY_MASK  0x3ff       // Mask off top 10 bits to get 0-1023 index

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
    uint32_t frameAddr          : 20; // Frame address (shifted right 12 bits)
};

/**
 * @brief Page table structure as defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
struct Table
{
   struct TableEntry pages[ARCH_PAGE_TABLE_ENTRIES]; // All entries for the table
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
    struct DirectoryEntry entries[ARCH_PAGE_DIR_ENTRIES]; // Pointers that the Intel CPU uses to access pages in memory
};
