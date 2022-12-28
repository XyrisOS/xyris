/**
 * @file cxa_pure_virtual.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-12-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#include <Panic.hpp>

extern "C"
{

// Function prototypes (to make compiler happy)
void __cxa_pure_virtual();

void __cxa_pure_virtual()
{
    panic("Pure virtual called!");
}

}
