#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <mem/MemoryTypes.hpp>

namespace Memory {

typedef struct {
    uintptr_t base;
    size_t length;
    MemoryType type;
} MapEntry;

typedef bool (*MapIter)(MapEntry *, void *ctx);

void PhysicalInit(size_t memSize, uintptr_t kernelEnd, MapIter mapIter, void *mapIterCtx);

size_t PhysicalGetPageSize(void);

size_t PhysicalFindFree(size_t count);

void PhysicalFree(size_t page, size_t count);

bool PhysicalIsAvailable(size_t page, size_t count);

void PhysicalAlloc(size_t page, size_t count);

}
