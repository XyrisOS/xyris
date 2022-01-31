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
#include <Applications/spinner.hpp>
#include <Devices/Graphics/console.hpp>
#include <Memory/Heap.hpp>
#include <Library/string.hpp>
#include <Arch/Memory.hpp>
#include <Panic.hpp>

namespace Apps {

void spinner(void) {
    Console::printf("\n");
    int i = 0;
    const char spinnay[] = { '|', '/', '-', '\\' };
    for (size_t size = 1; size < 1024 * 1024 * 1024; size++) {
        void* ptr = malloc(size);
        if (ptr == nullptr) {
            panic("null malloc in stress test!");
        }
        memset(ptr, 0xCAFEBABE, size / sizeof(0xCAFEBABE));
        // Display a spinner to know that we're still running.
        Console::printf("\e[s\e[24;0f%c Bytes: %zu\e[u", spinnay[i], size);
        i = (i + 1) % sizeof(spinnay);
        free(ptr);
    }
    Console::printf("Done!\n");
/*
    while (true) {
        // Display a spinner to know that we're still running.
        Console::printf("\e[s\e[24;0f%c\e[u", spinnay[i]);
        i = (i + 1) % sizeof(spinnay);
        asm volatile("hlt");
    }
*/
}

}
