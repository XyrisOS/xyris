/**
 * @file Loader.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2021-12-16
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bootloader to kernel stage 2. This stage is responsible
 * for handling any generic, catch all, preparations before calling
 * into the kernel's main function. Stage 2 should be bootloader
 * independent.
 *
 */
void stage2Entry(void* info, uint32_t magic);

#ifdef __cplusplus
}
#endif
