/**
 * @file trace.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Kernel stack tracing
 * @version 0.3
 * @date 2020-08-09
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once
#include <stddef.h>

namespace Stack {

// TODO: Take function to call for each frame
void printTrace(size_t max);

} // !namespace Stack
