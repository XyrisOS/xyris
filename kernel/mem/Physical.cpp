/**
 * @file Physical.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2021-10-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <lib/Bitset.hpp>
#include <mem/Physical.hpp>

namespace Memory::Physical {

PhysicalManager::PhysicalManager()
{
    // TODO: Initialize bitset and do other stuff
}

void PhysicalManager::SetFree(Section& sect)
{
    (void)sect;
}

void PhysicalManager::SetUsed(Section& sect)
{
    (void)sect;
}

void PhysicalManager::IsUsed(Section& sect)
{
    (void)sect;
}

void PhysicalManager::SetFree(uintptr_t addr)
{
    (void)addr;
}

void PhysicalManager::SetUsed(uintptr_t addr)
{
    (void)addr;
}

void PhysicalManager::IsUsed(uintptr_t addr)
{
    (void)addr;
}

}
