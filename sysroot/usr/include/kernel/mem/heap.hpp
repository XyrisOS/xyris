/**
 * @file heap.hpp
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

size_t px_kmalloc_a(size_t sz);                 // page aligned.
size_t px_kmalloc_p(size_t sz, size_t *phys);   // returns a physical address.
size_t px_kmalloc_ap(size_t sz, size_t *phys);  // page aligned and returns a physical address.
size_t px_kmalloc(size_t sz);                   // vanilla (normal).

#endif /* PANIX_MEM_ALLOC */