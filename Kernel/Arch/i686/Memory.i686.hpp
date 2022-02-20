/**
 * @file Memory.i686.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief i686 memory structures and definitions. C & C++ compatible header.
 * @version 0.1
 * @date 2021-10-26
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <stdint.h>
#include <stddef.h>
#ifndef __cplusplus
    // C needs bool type definition
    #include <stdbool.h>
#endif

#define ARCH_PAGE_DIR_ENTRIES       1024
#define ARCH_PAGE_TABLE_ENTRIES     1024
#define ARCH_PAGE_SIZE              4096
#define ARCH_PAGE_ALIGN             0xFFFFF000
#define ARCH_PAGE_DIR_ENTRY_SHIFT   22          // Shift to convert address to 0-1023 directory index
#define ARCH_PAGE_TABLE_ENTRY_SHIFT 12          // Shift to convert address to page address (2^12 = 4096 = PAGE_SIZE)
#define ARCH_PAGE_TABLE_ENTRY_MASK  0x3ff       // Mask off top 10 bits to get 0-1023 index

/* Only use namespace when including with C++ source so that the
   bootloader to kernel bootstrap source can access these structs
   It's a bit hacky since normally C++ includes C headers that have
   C++ include guards, but because Xyris is primarily C++ with a
   little bit of C, it makes more sense to do it this way */
#ifdef __cplusplus
namespace Arch::Memory {
#endif

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
    struct DirectoryEntry entries[1024]; // Pointers that the Intel CPU uses to access pages in memory
};

/* Only provide the Address class to C++ source so that this header can
   be included by the bootstrap C source */
#ifdef __cplusplus
/**
 * @brief Address class. Represents a memory address that can
 * be used to address a page in virtual memory, a frame in
 * physical memory. Allows setting the value of these addresses
 * by also including an uintptr_t representation.
 *
 */
class Address
{
public:
    Address(uintptr_t addr) {
        m_addr.val = addr;
    }

    Address(struct Frame frame) {
        m_addr.frame = frame;
    }

    Address(struct Page page) {
        m_addr.page = page;
    }

    operator struct Page() {
        return m_addr.page;
    }

    operator struct Frame() {
        return m_addr.frame;
    }

    operator uintptr_t() {
        return m_addr.val;
    }

    uintptr_t operator+= (const uintptr_t& val) {
        m_addr.val += val;
        return m_addr.val;
    }

    uintptr_t operator-= (const uintptr_t& val) {
        m_addr.val -= val;
        return m_addr.val;
    }

    uintptr_t operator= (const uintptr_t& val) {
        m_addr.val = val;
        return m_addr.val;
    }

    struct Page page() {
        return m_addr.page;
    }

    struct Frame frame() {
        return m_addr.frame;
    }

    uintptr_t val() {
        return m_addr.val;
    }

private:
    union {
        struct Page page;
        struct Frame frame;
        uintptr_t val;
    } m_addr;
};
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
static inline uintptr_t pageAlign(size_t addr)
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
static inline bool pageIsAligned(size_t addr)
{
    return ((addr % ARCH_PAGE_SIZE) == 0);
}

#ifdef __cplusplus
} // !namespace Arch::Memory
#endif
