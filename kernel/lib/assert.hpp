/**
 * @file assert.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-07-14
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

/**
 * @brief Creates an assertion which must be held to be true.
 * If the assertion passes, nothing happens and the kernel
 * continue to execute. However, in the even that the assert
 * fails and the value received is not the expected value,
 * the kernel will Panic with the message "Assert failed at"
 * followed by the file, line number, and function.
 *
 */
#define assert(x) (!(x) ? (PANIC("Assert failed at ")) : (void)0)
