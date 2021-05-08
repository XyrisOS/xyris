#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

namespace Memory {

typedef enum {
    MMAP_AVAILABLE,
    MMAP_RESERVED,
    MMAP_ACPI_NV,
    MMAP_ACPI_RECLAIMABLE
} MapType;

typedef struct {
    uintptr_t base;
    size_t length;
    MapType type;
} MapEntry;

typedef bool (*px_mmap_iter_t)(MapEntry *, void *ctx);

void PhysicalInit(size_t memSize, uintptr_t kernelEnd, px_mmap_iter_t mapIter, void *mapIterCtx);

size_t PhysicalGetPageSize(void);

size_t PhysicalFindFree(size_t count);

void PhysicalFree(size_t page, size_t count);

bool PhysicalIsAvailable(size_t page, size_t count);

void PhysicalAlloc(size_t page, size_t count);

}