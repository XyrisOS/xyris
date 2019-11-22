/**
 * @file alloc.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */

#ifndef PANIX_MEM_ALLOC
#define PANIX_MEM_ALLOC

#include <sys/sys.hpp>

uint32_t px_kmalloc_a(uint32_t sz);                     // page aligned.
uint32_t px_kmalloc_p(uint32_t sz, uint32_t *phys);     // returns a physical address.
uint32_t px_kmalloc_ap(uint32_t sz, uint32_t *phys);    // page aligned and returns a physical address.
uint32_t px_kmalloc(uint32_t sz);                       // vanilla (normal).

#endif /* PANIX_MEM_ALLOC */