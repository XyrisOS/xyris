#pragma once
#include <stddef.h>
#include <stdint.h>

namespace Memory {

typedef enum {
    MemoryUsable = 0,
    MemoryReserved,
    MemoryAcpiNVS,
    MemoryAcpiReclaimable,
    MemoryBootloader,
    MemoryKernel,
    MemoryBad,
} MemoryType;

}
