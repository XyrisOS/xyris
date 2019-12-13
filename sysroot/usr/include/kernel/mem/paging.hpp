/**
 * @file paging.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_MEM_PAGING
#define PANIX_MEM_PAGING

#include <sys/sys.hpp>
#include <arch/x86/isr.hpp>
#include <mem/heap.hpp>
/**
 * @brief Page table entry defined in accordance to the
 * Intel Developer Manual Vol. 3a p. 4-12
 * 
 */
typedef struct px_page_table_entry
{
   uint32_t present           : 1;  // Page present in memory
   uint32_t read_write        : 1;  // Read-only if clear, readwrite if set
   uint32_t usermode          : 1;  // Supervisor level only if clear
   uint32_t write_through     : 1;  // Page level write through
   uint32_t cache_disable     : 1;  // Disables TLB caching of page entry
   uint32_t accessed          : 1;  // Has the page been accessed since last refresh?
   uint32_t dirty             : 1;  // Has the page been written to since last refresh?
   uint32_t page_att_table    : 1;  // Page attribute table (memory cache control)
   uint32_t global            : 1;  // Prevents the TLB from updating the address
   uint32_t unused            : 3;  // Amalgamation of unused and reserved bits
   uint32_t frame             : 20; // Frame address (shifted right 12 bits)
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
typedef struct px_page_directory_entry {
   uint32_t present           : 1;  // Is the page present in physical memory?
   uint32_t read_write        : 1;  // Is the page read/write or read-only?
   uint32_t usermode          : 1;  // Can the page be accessed in usermode?
   uint32_t write_through     : 1;  // Is write-through cache enabled?
   uint32_t cache_disable     : 1;  // Can the page be cached?
   uint32_t accessed          : 1;  // Has the page been accessed?
   uint32_t ignored_a         : 1;  // Ignored
   uint32_t page_size         : 1;  // Is the page 4 Mb (enabled) or 4 Kb (disabled)?
   uint32_t ignored_b         : 4;  // Ignored
   uint32_t table_addr        : 20; // Physical address of the table
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
} page_directory_t;

/**
  Sets up the environment, page directories etc and
  enables paging.
**/
void px_paging_init();

/**
  Causes the specified page directory to be loaded into the
  CR3 register.
**/
void px_mem_switch_page_directory(page_directory_t* newPage);

/**
  Retrieves a pointer to the page required.
  If make == 1, if the page-table in which this page should
  reside isn't created, create it!
**/
px_page_table_entry_t *px_mem_get_page(uint32_t address, int make, page_directory_t *dir);

/**
  Handler for page faults.
**/
void px_mem_page_fault(registers_t regs);

#endif /* PANIX_MEM_PAGING */