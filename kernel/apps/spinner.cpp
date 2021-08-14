/**
 * @file spinner.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Kernel spinner task
 * @version 0.1
 * @date 2021-06-05
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <apps/spinner.hpp>
#include <stdint.h>
#include <lib/stdio.hpp>

namespace apps {

void spinner(void) {
    kprintf("\n");
    int i = 0;
    const char spinnay[] = { '|', '/', '-', '\\' };
    while (true) {
        // Display a spinner to know that we're still running.
        kprintf("\e[s\e[24;0f%c\e[u", spinnay[i]);
        i = (i + 1) % sizeof(spinnay);
        asm volatile("hlt");
    }
}

}
