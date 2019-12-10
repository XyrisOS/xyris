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

uint32_t placement_address;

size_t px_kmalloc_internal(size_t sz, int align, uint32_t *phys) {
    // If the address is not already page-aligned
    if (align == true && (placement_address & 0xFFFFF000)) {
        // Align it.
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (phys) {
        *phys = placement_address;
    }
    size_t tmp = placement_address;
    placement_address += sz;
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