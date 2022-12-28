/**
 * @file terminate.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Provides an implementation of `std::terminate` for cxa support methods.
 * @version 0.1
 * @date 2022-12-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#include <Panic.hpp>

namespace std {

[[noreturn]] void terminate()
{
    panic("std::terminate called!");
}

} // !namespace std
