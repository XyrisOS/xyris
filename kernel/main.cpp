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
// Generic devices
#include <dev/tty/tty.hpp>
#include <dev/kbd/kbd.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/spkr/spkr.hpp>
#include <dev/serial/rs232.hpp>
// Apps
#include <apps/primes.hpp>
#include <apps/spinner.hpp>

static void kernel_print_splash();
static void kernel_boot_tone();

static void print_boot_info(void *boot_info, uint32_t magic)
{
    rs232_printf("Bootloader info at 0x%x\n", boot_info);
    if (boot_info != NULL) {
        uintptr_t page = (uintptr_t)boot_info & PAGE_ALIGN;
        map_kernel_page(VADDR(page), page);
    }
    const char *boot_proto_name = "Unknown";
    if (magic == 0x2BADB002) {
        boot_proto_name = "Multiboot 1";
    } else if (magic == 0x36d76289) {
        boot_proto_name = "Multiboot 2";
        parse_multiboot2(boot_info);
    } else if (magic == *(uint32_t*)"stv2") {
        boot_proto_name = "Stivale 2";
        parse_stivale2(boot_info);
    }
    kprintf(DBG_INFO "Booted via %s\n", boot_proto_name);
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/i386/boot.S.
 */
void kernel_main(void *boot_info, uint32_t magic) {
    (void)boot_info;
    // Print the splash screen to show we've booted into the kernel properly.
    kernel_print_splash();
    // Install the GDT
    interrupts_disable();
    gdt_install();           // Initialize the Global Descriptor Table
    isr_install();           // Initialize Interrupt Service Requests
    rs232_init(RS_232_COM1); // RS232 Serial
    paging_init(0);          // Initialize paging service (0 is placeholder)
    print_boot_info(boot_info, magic);
    kbd_init();              // Initialize PS/2 Keyboard
    rtc_init();              // Initialize Real Time Clock
    timer_init(1000);        // Programmable Interrupt Timer (1ms)
    // Enable interrupts now that we're out of a critical area
    interrupts_enable();
    // Enable serial input
    rs232_init_buffer(1024);
    // Print some info to show we did things right
    rtc_print();
    // Get the CPU vendor and model data to print
    const char *vendor = cpu_get_vendor();
    const char *model = cpu_get_model();
    kprintf(DBG_INFO "%s\n", vendor);
    kprintf(DBG_INFO "%s\n", model);
    // Print out the CPU vendor info
    rs232_print(vendor);
    rs232_print("\n");
    rs232_print(model);
    rs232_print("\n");

    tasks_init();
    task_t compute, status, spinner;
    tasks_new(apps::find_primes, &compute, TASK_READY, "prime_compute");
    tasks_new(apps::show_primes, &status, TASK_READY, "prime_display");
    tasks_new(apps::spinner, &spinner, TASK_READY, "spinner");

    // Now that we're done make a joyful noise
    kernel_boot_tone();

    // Keep the kernel task alive.
    tasks_block_current(TASK_PAUSED);
    PANIC("Kernel terminated unexpectedly!");
}

static void kernel_print_splash() {
    tty_clear();
    kprintf(
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
    kprintf("Commit %s (v%s.%s.%s) built on %s at %s.\n\n", COMMIT, VER_MAJOR, VER_MINOR, VER_PATCH, __DATE__, __TIME__);
}

static void kernel_boot_tone() {
    // Beep beep!
    spkr_beep(1000, 50);
    sleep(100);
    spkr_beep(1000, 50);
}
