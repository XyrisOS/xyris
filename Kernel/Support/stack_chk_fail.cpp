/**
 * @file stack_chk_fail.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-12-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#include <Panic.hpp>

/**
 * @brief This function is the global handler for all stack protection. GCC will automatically
 * write the canary code and use this function as the handler for when a smashed stack is detected.
 *
 */
#if UINTPTR_MAX == UINT32_MAX
uintptr_t __stack_chk_guard = 0xDEADC0DE;
#elif UINTPTR_MAX == UINT64_MAX
uintptr_t __stack_chk_guard = 0xBADBADBADBADBAD1;
#endif

extern "C"
{

// Function prototypes (to make compiler happy)
void __stack_chk_fail(void);

void __stack_chk_fail(void)
{
    panic("Smashed stack detected.");
}

}
