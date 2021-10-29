/**
 * @file Physical.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Physical memory mapping
 * @version 0.1
 * @date 2021-10-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once

#include <arch/Memory.hpp>
#include <mem/MemorySection.hpp>
#include <stddef.h>
#include <stdint.h>

namespace Memory::Physical {

class PhysicalManager {
public:
    PhysicalManager();

    void SetFree(Section& sect);
    void SetUsed(Section& sect);
    void IsUsed(Section& sect);

    void SetFree(uintptr_t addr);
    void SetUsed(uintptr_t addr);
    void IsUsed(uintptr_t addr);

private:
};

}
