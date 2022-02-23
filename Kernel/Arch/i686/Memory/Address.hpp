/**
 * @file Address.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief i686 memory address helper class
 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <Arch/i686/Memory/Types.h>

namespace Arch::Memory {

/**
 * @brief Address class. Represents a memory address that can
 * be used to address a page in virtual memory, a frame in
 * physical memory. Allows setting the value of these addresses
 * by also including an uintptr_t representation.
 *
 */
class Address {
public:
    Address(uintptr_t addr)
    {
        m_addr.val = addr;
    }

    Address(struct Page page)
    {
        m_addr.page = page;
    }

    Address(struct VirtualAddress vaddr)
    {
        m_addr.vaddr = vaddr;
    }

    operator struct VirtualAddress()
    {
        return m_addr.vaddr;
    }

    operator struct Page()
    {
        return m_addr.page;
    }

    operator uintptr_t()
    {
        return m_addr.val;
    }

    uintptr_t operator+=(const uintptr_t& val)
    {
        m_addr.val += val;
        return m_addr.val;
    }

    uintptr_t operator-=(const uintptr_t& val)
    {
        m_addr.val -= val;
        return m_addr.val;
    }

    uintptr_t operator=(const uintptr_t& val)
    {
        m_addr.val = val;
        return m_addr.val;
    }

    struct VirtualAddress virtualAddress()
    {
        return m_addr.vaddr;
    }

    struct Page page()
    {
        return m_addr.page;
    }

    uintptr_t val()
    {
        return m_addr.val;
    }

private:
    union {
        struct VirtualAddress vaddr;
        struct Page page;
        uintptr_t val;
    } m_addr;
};

} // !namespace Arch::Memory
