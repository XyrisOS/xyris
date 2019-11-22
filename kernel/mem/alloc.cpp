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

#include <mem/alloc.hpp>

uint32_t placement_address;

uint32_t px_kmalloc(uint32_t sz, int align, uint32_t *phys) {
    // If the address is not already page-aligned
    if (align == 1 && (placement_address & 0xFFFFF000)) {
        // Align it.
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (phys) {
        *phys = placement_address;
    }
    uint32_t tmp = placement_address;
    placement_address += sz;
    return tmp;
}