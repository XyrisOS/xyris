/**
 * @file Flush.h
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Header for assembly functions defined in flush.s
 * @version 0.1
 * @date 2022-01-03
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#include <stdint.h>

extern "C" {

/**
 * @brief Flush the global descriptor table and use the one provided
 *
 * @param gdt Address of global descriptor table to be used
 */
void gdt_flush(uintptr_t gdt);

/**
 * @brief Flush the TSS when we (re)load into the kernel or into userspace.
 *
 */
void tss_flush(void);

}
