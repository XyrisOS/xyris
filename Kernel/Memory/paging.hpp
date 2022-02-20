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

#include <Arch/Memory.hpp>
#include <Memory/MemoryMap.hpp>
#include <stddef.h>
#include <stdint.h>

namespace Memory {

/**
 * @brief Sets up the environment, page directories etc and enables paging.
 *
 */
void init(MemoryMap* map);

/**
 * @brief Returns a new page in memory for use.
 * If less than one page is requested, exactly one page
 * will be allocated and returned.
 *
 * @param size Page size in bytes
 * @return void* Page memory address
 */
void* newPage(size_t size);

// TODO: Docs
void* newPageMustSucceed(size_t size);


/**
 * @brief Frees pages starting at a given page address.
 *
 * @param page Starting location of page(s) to be freed
 * @param size Number of bytes to be freed
 */
void freePage(void* page, size_t size);

/**
 * @brief Checks whether an address is mapped into memory.
 *
 * @param addr Address to be checked.
 * @return true The address is mapped in and valid.
 * @return false The address is not mapped into memory.
 */
bool isPresent(uintptr_t addr);

/**
 * @brief Gets the physical address of the current page directory.
 *
 * @returns the physical address of the current page directory.
 */
uintptr_t getPageDirPhysAddr();

/**
 * @brief Map a page into the kernel address space.
 *
 * @param vaddr Virtual address (in kernel space)
 * @param paddr Physical address
 */
void mapKernelPage(Arch::Memory::Address vaddr, Arch::Memory::Address paddr);

/**
 * @brief Map an address range into the kernel virtual address space.
 *
 * @param sect Memory section
 */
void mapKernelRangeVirtual(Section sect);

/**
 * @brief Map a kernel address range into physical memory.
 *
 * @param sect Memory section
 */
void mapKernelRangePhysical(Section sect);

} // !namespace Paging
