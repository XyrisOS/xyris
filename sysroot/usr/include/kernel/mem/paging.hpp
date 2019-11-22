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
#include <mem/alloc.hpp>

typedef struct page
{
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table
{
   page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
   /**
      Array of pointers to pagetables.
   **/
   page_table_t *tables[1024];
   /**
      Array of pointers to the pagetables above, but gives their *physical*
      location, for loading into the CR3 register.
   **/
   uint32_t tablesPhysical[1024];
   /**
      The physical address of tablesPhysical. This comes into play
      when we get our kernel heap allocated and the directory
      may be in a different location in virtual memory.
   **/
   uint32_t physicalAddr;
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
page_t *px_mem_get_page(uint32_t address, int make, page_directory_t *dir);

/**
  Handler for page faults.
**/
void px_mem_page_fault(registers_t regs);

#endif /* PANIX_MEM_PAGING */