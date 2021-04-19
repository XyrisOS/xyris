/**
 * @file main.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The entry point into the Panix kernel. Everything is loaded from here.
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */
// System library functions
#include <stdint.h>
#include <sys/kernel.hpp>
#include <sys/panic.hpp>
#include <sys/tasks.hpp>
#include <lib/string.hpp>
#include <lib/stdio.hpp>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// Architecture specific code
#include <arch/arch.hpp>
#include <arch/BootInfo.hpp>
// Generic devices
#include <dev/tty/tty.hpp>
#include <dev/kbd/kbd.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/spkr/spkr.hpp>
#include <dev/serial/rs232.hpp>
// Apps
#include <apps/primes.hpp>

static void px_kernel_print_splash();
static void px_kernel_boot_tone();

static void px_init_bootinfo(void *boot_info, uint32_t magic)
{
    BootInfo handle = BootInfo();
    handle.magic = magic;
    handle.payload = boot_info;

    px_rs232_printf("Bootloader info at 0x%x\n", boot_info);
    if (boot_info != NULL) {
        uintptr_t page = (uintptr_t)boot_info & PAGE_ALIGN;
        px_map_kernel_page(VADDR(page), page);
    }
    const char *boot_proto_name = "Unknown";
    if (magic == 0x2BADB002) {
        boot_proto_name = "Multiboot 1";
    } else if (magic == 0x36d76289) {
        boot_proto_name = "Multiboot 2";
        px_parse_multiboot2(&handle, boot_info);
    } else if (magic == *(uint32_t*)"stv2") {
        boot_proto_name = "Stivale 2";
        px_parse_stivale2(&handle, boot_info);
    }
    px_kprintf(DBG_INFO "Booted via %s\n", boot_proto_name);
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/i386/boot.S.
 */
void px_kernel_main(void *boot_info, uint32_t magic) {
    // Print the splash screen to show we've booted into the kernel properly.
    px_kernel_print_splash();
    // Initialize the CPU
    px_arch_main();
    // Initialize drivers
    px_paging_init(0);          // Initialize paging service (0 is placeholder)
    px_rs232_init(RS_232_COM1); // RS232 Serial
    px_rs232_init_buffer(1024); // RS232 buffer
    px_kbd_init();              // Initialize PS/2 Keyboard
    px_rtc_init();              // Initialize Real Time Clock
    // Parse information from bootloader
    px_init_bootinfo(boot_info, magic);
    // Print some info to show we did things right
    px_rtc_print();
    // Get the CPU vendor and model data to print
    char *vendor = (char *)px_cpu_get_vendor();
    char *model = (char *)px_cpu_get_model();
    px_kprintf(DBG_INFO "%s\n", vendor);
    px_kprintf(DBG_INFO "%s\n", model);
    // Start the serial debugger
    px_kprintf(DBG_INFO "Starting serial debugger...\n");
    // Print out the CPU vendor info
    px_rs232_print(vendor);
    px_rs232_print("\n");
    px_rs232_print(model);
    px_rs232_print("\n");

    // Done
    px_kprintf(DBG_OKAY "Done.\n");

    px_tasks_init();
    px_task_t compute, status;
    px_tasks_new(find_primes, &compute, TASK_READY, "prime_compute");
    px_tasks_new(show_primes, &status, TASK_READY, "prime_display");

    // Now that we're done make a joyful noise
    px_kernel_boot_tone();

    // Keep the kernel alive.
    px_kprintf("\n");
    int i = 0;
    const char spinnay[] = { '|', '/', '-', '\\' };
    while (true) {
        // Display a spinner to know that we're still running.
        px_kprintf("\e[s\e[24;0f%c\e[u", spinnay[i]);
        i = (i + 1) % sizeof(spinnay);
        asm volatile("hlt");
    }
    PANIC("Kernel terminated unexpectedly!");
}

static void px_kernel_print_splash() {
    px_tty_clear();
    px_kprintf(
        "\033[93mWelcome to Panix %s\n"
        "Developed by graduates and undergraduates of Cedarville University.\n"
        "Copyright the Panix Contributors (c) %i. All rights reserved.\n\033[0m",
        VER_NAME,
        (
            ((__DATE__)[7] - '0') * 1000 + \
            ((__DATE__)[8] - '0') * 100  + \
            ((__DATE__)[9] - '0') * 10   + \
            ((__DATE__)[10] - '0') * 1     \
        )
    );
    px_kprintf("Commit %s (v%s.%s.%s) built on %s at %s.\n\n", COMMIT, VER_MAJOR, VER_MINOR, VER_PATCH, __DATE__, __TIME__);
}

static void px_kernel_boot_tone() {
    // Beep beep!
    px_spkr_beep(1000, 50);
    sleep(100);
    px_spkr_beep(1000, 50);
}
