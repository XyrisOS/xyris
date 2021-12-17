/**
 * @file EarlyPanic.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Early CGA-mode panic (no framebuffer)
 * @version 0.1
 * @date 2021-12-16
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Prints a panic message to the screen (in CGA-mode only)
 * and stops CPU execution.
 *
 */
__attribute__((noreturn))
void EarlyPanic(const char *str);

#ifdef __cplusplus
}
#endif
