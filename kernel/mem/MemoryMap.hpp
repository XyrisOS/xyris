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

    /**
     * @brief Returns the max number of memory sections allowed in a MemoryMap
     *
     * @return size_t Max number of entries
     */
    size_t Count()
    {
        return m_max_sections;
    }

    /**
     * @brief Returns a reference to the memory section at the given index
     *
     * @param idx Memory section index
     * @return Section& Section at index available for updating
     */
    Section& Get(size_t idx)
    {
        return m_sections[idx];
    }

    /**
     * @brief Getter bracket operator. Returns a copy of the Memory::Section at
     * the provided index.
     *
     * @param idx Memory section index
     * @return Section Copy of the Memory::Section
     */
    Section operator[](size_t idx) const
    {
        return m_sections[idx];
    }

    /**
     * @brief Getter bracket operator. Returns a reference to the Memory::Section at
     * the provided index.
     *
     * @param idx Memory section index
     * @return Section& Reference to the Memory::Section
     */
    Section& operator[](size_t idx)
    {
        return m_sections[idx];
    }

private:
    // TODO: Set some sort of counter that indicates how many were inserted
    static const size_t m_max_sections = 32;
    Section m_sections[m_max_sections];
};

} // !namespace Memory
