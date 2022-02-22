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

    Address(struct Frame frame)
    {
        m_addr.frame = frame;
    }

    Address(struct VirtualAddress page)
    {
        m_addr.page = page;
    }

    operator struct VirtualAddress()
    {
        return m_addr.page;
    }

    operator struct Frame()
    {
        return m_addr.frame;
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
        return m_addr.page;
    }

    struct Frame frame()
    {
        return m_addr.frame;
    }

    uintptr_t val()
    {
        return m_addr.val;
    }

private:
    union {
        struct VirtualAddress page;
        struct Frame frame;
        uintptr_t val;
    } m_addr;
};

} // !namespace Arch::Memory
