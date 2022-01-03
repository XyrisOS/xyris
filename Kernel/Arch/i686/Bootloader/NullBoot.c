/**
 * @file NullBoot.c
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Kernel ELF entry point. Should never be called by any bootloader.
 * @version 0.1
 * @date 2021-12-29
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */

void _start(void);

/**
 * @brief ELF default entry point. Should never be called since a bootloader
 * should always have a custom entry point specific to that bootloader.
 *
 */
__attribute__((section(".early_text")))
void _start(void)
{
    return;
}
