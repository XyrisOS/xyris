/**
 * @file MemoryMap.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Memory map containing a number of Memory sections
 * that describe the physical memory layout
 * @version 0.1
 * @date 2021-09-12
 * 
 * @copyright Copyright the Xyris Contributors (c) 2021
 * 
 */
#pragma once
#include <mem/MemorySection.hpp>

namespace Memory {

class MemoryMap {
public:
    MemoryMap() = default;
    size_t Count()
    {
        return m_max_sections;
    }
    Section& Get(size_t idx)
    {
        return m_sections[idx];
    }
    Section operator[](size_t idx) const
    {
        return m_sections[idx];
    }
    Section& operator[](size_t idx)
    {
        return m_sections[idx];
    }

private:
    static const size_t m_max_sections = 32;
    Section m_sections[m_max_sections];
};

} // !namespace Memory
