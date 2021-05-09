/**
 * @file i386.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Intel i386 specific headers, functions, & structs
 * @version 0.1
 * @date 2021-04-18
 *
 * @copyright Copyright the Panix Contributors (c) 2021
 *
 */
#pragma once
#include <stdint.h>
#include <stddef.h>

// i386 architecture includes
#include <arch/i386/gdt.hpp>
#include <arch/i386/idt.hpp>
#include <arch/i386/isr.hpp>
#include <arch/i386/timer.hpp>
#include <arch/i386/ports.hpp>

/**
 * @brief x86 BIOS based VGA pointers and data.
 * Used for printing to the VGA screen via BIOS memory.
 */
#define X86_TTY_WIDTH   80
#define X86_TTY_HEIGHT  25
#define X86_IND_X       79
#define X86_IND_Y       0
inline uint16_t* x86_bios_vga_mem = (uint16_t*) 0x000B8000;

/**
 * @brief x86 specific paging definitions and functions.
 */
namespace Memory {
#define PAGE_SIZE       0x1000

/**
 * @brief Provides a structure for defining the necessary fields
 * which comprise a virtual address.
 */
typedef union
{
    struct {
        uint32_t offset         : 12;   // Page offset address
        uint32_t tableIndex     : 10;   // Page table entry
        uint32_t dirIndex       : 10;   // Page directory entry
    };
    uint32_t val;
} VirtualAddress;

/**
 * @brief Page table entry defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
typedef struct
{
    uint32_t present            : 1;    // Page present in memory
    uint32_t readWrite          : 1;    // Read-only if clear, readwrite if set
    uint32_t usermode           : 1;    // Supervisor level only if clear
    uint32_t writeThrough       : 1;    // Page level write through
    uint32_t cacheDisable       : 1;    // Disables TLB caching of page entry
    uint32_t accessed           : 1;    // Has the page been accessed since last refresh?
    uint32_t dirty              : 1;    // Has the page been written to since last refresh?
    uint32_t pageAttributeTable : 1;    // Page attribute table (memory cache control)
    uint32_t global             : 1;    // Prevents the TLB from updating the address
    uint32_t unused             : 3;    // Amalgamation of unused and reserved bits
    uint32_t frame              : 20;   // Frame address (shifted right 12 bits)
} PageTableEntry;

/**
 * @brief Page table structure as defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
typedef struct
{
   PageTableEntry pages[1024];
} PageTable;

/**
 * @brief Page directory entry structure as defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
typedef struct
{
    uint32_t present            : 1;    // Is the page present in physical memory?
    uint32_t readWrite          : 1;    // Is the page read/write or read-only?
    uint32_t usermode           : 1;    // Can the page be accessed in usermode?
    uint32_t writeThrough       : 1;    // Is write-through cache enabled?
    uint32_t cacheDisable       : 1;    // Can the page be cached?
    uint32_t accessed           : 1;    // Has the page been accessed?
    uint32_t ignoredA           : 1;    // Ignored
    uint32_t pageSize           : 1;    // Is the page 4 Mb (enabled) or 4 Kb (disabled)?
    uint32_t ignoredB           : 4;    // Ignored
    uint32_t tableAddr          : 20;   // Physical address of the table
} PageDirectoryEntry;

/**
 * @brief Page directory contains pointers to all of the virtual memory addresses for the
 * page tables along with their corresponding physical memory locations of the page tables.
 * Page table entry defined in accordance to the Intel Developer Manual Vol. 3a p. 4-12.
 *
 */
typedef struct
{
    PageTable *tables[1024];                    // Pointers that Panix uses to access the pages in memory
    PageDirectoryEntry tablesPhysical[1024];    // Pointers that the Intel CPU uses to access pages in memory
    uint32_t physicalAddr;                      // Physical address of this 4Kb aligned page table referenced by this entry
} PageDirectory;

inline void SetPageDirectory(size_t pageDir);
inline void PagingEnable();
inline void PagingDisable();

} // namespace Memory

/**
 * @brief A structure definining values for every since x86 register.
 * Used when in various x86 architecture functions and panic.
 */
typedef struct registers {
    uint32_t ds;                                          /* Data segment selector */
    uint32_t edi, esi, ebp, ignored, ebx, edx, ecx, eax;  /* Pushed by pusha. */
    uint32_t int_num, err_code;                           /* Interrupt number and error code (if applicable) */
    uint32_t eip, cs, eflags, esp, ss;                    /* Pushed by the processor automatically */
} registers_t;

struct stackframe {
  struct stackframe* ebp;
  size_t eip;
};