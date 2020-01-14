/**
 * @file alloc.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#include <mem/heap.hpp>

bool initialized = false;
uint32_t base_init = 0x0;
uint32_t base_curr = 0x0;
uint32_t base_max = 0x0;

void px_heap_init(uint32_t base, uint32_t max) {
    assert(base < max);
    px_print_debug("Initializing heap...", Info);
    // Set the init and curr values equal to the base
    base_init = base_curr = base;
    base_max = max;
    initialized = true;
}

size_t px_kmalloc_internal(size_t sz, int align, uint32_t *phys) {
    // If the paging is not initialized before this call then panic
    if (!initialized) {
        PANIC("Paging function called before init!");
    // If our size is 0 then do nothing
    } else if (!sz) {
        px_print_debug("Cannot allocate page of size 0.", Error);
        return 0;
    // If we're out of usable memory then panic
    } else if (sz + base_curr > base_max) {
        PANIC("Out of usable memory!");
    }
    // If we need to align the page
    if (align == true && base_curr & ~PAGE_ALIGN) {
        base_curr &= PAGE_ALIGN;
        base_curr += PAGE_SIZE;
    }
    // If we're assigning it to a physical address
    if (phys) {
        *phys = base_curr;
    }
    size_t tmp = base_curr;
    base_curr += sz;
    return tmp;
}

size_t px_kmalloc_a(size_t sz) {
    return px_kmalloc_internal(sz, true, nullptr);
}

size_t px_kmalloc_p(size_t sz, size_t *phys) {
    return px_kmalloc_internal(sz, false, phys);
}

size_t px_kmalloc_ap(size_t sz, size_t *phys) {
    return px_kmalloc_internal(sz, true, phys);
}

size_t px_kmalloc(size_t sz) {
    return px_kmalloc_internal(sz, false, nullptr);
}