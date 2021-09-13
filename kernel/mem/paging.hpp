/**
 * @file paging.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-22
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#pragma once

#include <arch/Arch.hpp>
#include <mem/MemoryMap.hpp>
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE   0x1000
#define PAGE_ALIGN  0xfffff000

namespace Paging {

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
 * @brief Sets up the environment, page directories etc and enables paging.
 *
 */
void init(Memory::MemoryMap* map);
/**
 * @brief Returns a new page in memory for use.
 *
 * @param size Page size in bytes
 * @return void* Page memory address
 */
void* newPage(uint32_t size);
/**
 * @brief Frees pages starting at a given page address.
 *
 * @param page Starting location of page(s) to be freed
 * @param size Number of bytes to be freed
 */
void freePage(void* page, uint32_t size);
/**
 * @brief Checks whether an address is mapped into memory.
 *
 * @param addr Address to be checked.
 * @return true The address is mapped in and valid.
 * @return false The address is not mapped into memory.
 */
bool isPresent(size_t addr);
/**
 * @brief Aligns the provided address to the start of its corresponding
 * page address.
 *
 * @param addr Address to be aligned
 * @return uintptr_t Page aligned address value
 */
uintptr_t alignAddress(uintptr_t addr);
/**
 * @brief Gets the physical address of the current page directory.
 *
 * @returns the physical address of the current page directory.
 */
uint32_t getPageDirPhysAddr();
/**
 * @brief Map a page into the kernel address space.
 *
 * @param vaddr Virtual address (in kernel space)
 * @param paddr Physical address
 */
void mapKernelPage(union Address vaddr, union Address paddr);
/**
 * @brief Map an address range into the kernel virtual address space.
 *
 * @param begin Beginning address
 * @param end Ending address
 */
void mapKernelRangeVirtual(uintptr_t begin, uintptr_t end);
/**
 * @brief Map a kernel address range into physical memory.
 *
 * @param begin Beginning address
 * @param end Ending address
 */
void mapKernelRangePhysical(uintptr_t begin, uintptr_t end);

} // !namespace Paging
