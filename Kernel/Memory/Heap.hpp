/**
 * @file Heap.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-01-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <stddef.h>

namespace Memory::Heap {

/**
 * @brief Initialize kernel heap. Randomizes the heap integrity magic
 * values and should only be called once.
 *
 */
void initialize();

/**
 * @brief Get the total number of bytes allocated on the heap.
 *
 * @return size_t Number of allocated bytes
 */
size_t getTotalAllocated();

/**
 * @brief Get the total number of bytes allocated and in use on the heap.
 *
 * @return size_t Number of bytes allocated and in use.
 */
size_t getTotalInUse();

} // !namespace Memory::Heap

void free(void* ptr);
void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
