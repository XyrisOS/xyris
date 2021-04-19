/**
 * @file paging.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-22
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <arch/arch.hpp>
#include <mem/heap.hpp>

// Information about the Kernel from the linker
extern uint32_t _KERNEL_START;
extern uint32_t _KERNEL_END;
extern "C" void px_invalidate_page(void *page_addr);

// Thanks to Grant Hernandez for uOS and the absolutely amazing code
// that he wrote. It helped us fix a lot of bugs and has provided a
// lot of quality of life defines such as the ones below that we would
// not have thought to use otherwise.
#define KERNEL_START        ((uint32_t)&_KERNEL_START)
#define KERNEL_END          ((uint32_t)&_KERNEL_END)
#define KERNEL_BASE         0xC0000000
#define ADDRESS_SPACE_SIZE  0x100000000
#define PAGE_SIZE           0x1000
#define PAGE_ALIGN          0xfffff000
#define PAGE_ALIGNED(addr)  (addr % PAGE_SIZE == 0)
#define NOT_PAGE_ALIGN      ~(PAGE_ALIGN)
#define PAGE_ALIGN_UP(addr) (((addr) & NOT_PAGE_ALIGN) ? (((addr) & PAGE_ALIGN) + PAGE_SIZE) : ((addr)))
#define PAGE_ALIGN_DOWN(addr) ((addr) & NOT_PAGE_ALIGN)
#define PAGE_ENTRY_PRESENT  0x1
#define PAGE_ENTRY_RW       0x2
#define PAGE_ENTRY_ACCESS   0x20
#define PAGE_ENTRIES        1024
#define PAGE_TABLE_SIZE     (sizeof(uint32)*PAGE_ENTRIES)
#define PAGES_PER_KB(kb)    (PAGE_ALIGN_UP((kb) * 1024) / PAGE_SIZE)
#define PAGES_PER_MB(mb)    (PAGE_ALIGN_UP((mb) * 1024 * 1024) / PAGE_SIZE)
#define PAGES_PER_GB(gb)    (PAGE_ALIGN_UP((gb) * 1024 * 1024 * 1024) / PAGE_SIZE)
#define VADDR(ADDR)         ((px_virtual_address_t){ .val = (ADDR) })

/**
 * @brief Provides a structure for defining the necessary fields
 * which comprise a virtual address.
 */
typedef union px_virtual_address
{
    struct {
        uint32_t page_offset       : 12;  // Page offset address
        uint32_t page_table_index  : 10;  // Page table entry
        uint32_t page_dir_index    : 10;  // Page directory entry
    };
    uint32_t val;
} px_virtual_address_t;

/**
 * @brief Page table entry defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
typedef struct px_page_table_entry
{
    uint32_t present            : 1;  // Page present in memory
    uint32_t read_write         : 1;  // Read-only if clear, readwrite if set
    uint32_t usermode           : 1;  // Supervisor level only if clear
    uint32_t write_through      : 1;  // Page level write through
    uint32_t cache_disable      : 1;  // Disables TLB caching of page entry
    uint32_t accessed           : 1;  // Has the page been accessed since last refresh?
    uint32_t dirty              : 1;  // Has the page been written to since last refresh?
    uint32_t page_att_table     : 1;  // Page attribute table (memory cache control)
    uint32_t global             : 1;  // Prevents the TLB from updating the address
    uint32_t unused             : 3;  // Amalgamation of unused and reserved bits
    uint32_t frame              : 20; // Frame address (shifted right 12 bits)
} px_page_table_entry_t;

/**
 * @brief Page table structure as defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
typedef struct px_page_table
{
   px_page_table_entry_t pages[1024];
} px_page_table_t;

/**
 * @brief Page directory entry structure as defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 *
 */
typedef struct px_page_directory_entry
{
    uint32_t present            : 1;  // Is the page present in physical memory?
    uint32_t read_write         : 1;  // Is the page read/write or read-only?
    uint32_t usermode           : 1;  // Can the page be accessed in usermode?
    uint32_t write_through      : 1;  // Is write-through cache enabled?
    uint32_t cache_disable      : 1;  // Can the page be cached?
    uint32_t accessed           : 1;  // Has the page been accessed?
    uint32_t ignored_a          : 1;  // Ignored
    uint32_t page_size          : 1;  // Is the page 4 Mb (enabled) or 4 Kb (disabled)?
    uint32_t ignored_b          : 4;  // Ignored
    uint32_t table_addr         : 20; // Physical address of the table
} px_page_directory_entry_t;

/**
 * @brief Page directory contains pointers to all of the virtual memory addresses for the
 * page tables along with their corresponding physical memory locations of the page tables.
 * Page table entry defined in accordance to the Intel Developer Manual Vol. 3a p. 4-12.
 *
 */
typedef struct px_page_directory
{
    px_page_table_t *tables[1024];                  // Pointers that Panix uses to access the pages in memory
    px_page_directory_entry_t tablesPhysical[1024]; // Pointers that the Intel CPU uses to access pages in memory
    uint32_t physical_addr;                         // Physical address of this 4Kb aligned page table referenced by this entry
} px_page_directory_t;

/**
 * @brief Sets up the environment, page directories etc and enables paging.
 *
 */
void px_paging_init(uint32_t num_pages);

/**
 * @brief Returns a new page in memory for use.
 *
 * @param size Page size in bytes
 * @return void* Page memory address
 */
void* px_get_new_page(uint32_t size);

/**
 * @brief Frees pages starting at a given page address.
 * 
 * @param page Starting location of page(s) to be freed
 * @param size Number of bytes to be freed
 */
void  px_free_page(void *page, uint32_t size);

/**
 * @brief Checks whether an address is mapped into memory.
 * 
 * @param addr Address to be checked.
 * @return true The address is mapped in and valid.
 * @return false The address is not mapped into memory.
 */
bool px_page_is_present(size_t addr);

/**
 * @brief Gets the physical address of the current page directory.
 * 
 * @returns the physical address of the current page directory.
 */
uint32_t px_get_phys_page_dir();

void px_map_kernel_page(px_virtual_address_t vaddr, uint32_t paddr);
