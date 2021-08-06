/**
 * @file main.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The entry point into the Xyris kernel.
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
// System library functions
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <lib/time.hpp>
#include <stdint.h>
#include <sys/tasks.hpp>
// Bootloader
#include <boot/Handoff.hpp>
// Architecture specific code
#include <arch/arch.hpp>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// Generic devices
#include <dev/graphics/framebuffer.hpp>
#include <dev/graphics/font.hpp>
#include <dev/graphics/graphics.hpp>
#include <dev/graphics/console.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/serial/rs232.hpp>
#include <dev/spkr/spkr.hpp>
// Apps
#include <apps/animation.hpp>
#include <apps/primes.hpp>
#include <apps/spinner.hpp>
// Debug
#include <lib/assert.hpp>
// Meta
#include <meta/defines.hpp>

static void printSplash();
static void bootTone();

// TODO: Rename this? bootInit is a bit much for what this does...
//       Maybe we should have a bootServiceInit or something elsewhere.
static void bootInit(void* boot_info, uint32_t magic, Boot::Handoff* handoff)
{
    // Ensure handoff is no longer default initialized
    *handoff = Boot::Handoff(boot_info, magic);
    assert(handoff->getHandle());
}

// TODO: Find a better way of doing this in the future
//       Maybe some sort of way to register driver init
//       functions that can be walked and called?
static void devInit()
{
    RS232::init(RS_232_COM1);
    RTC::init();
}

static void printSplash()
{
    kprintf(
        "\033[93m"
        "Xyris %s\n"
        "Copyright the Xyris Contributors (c) %i. All rights reserved.\n"
        "Kernel source available at %s.\n"
        "\033[0m",
        VER_NAME,
        (
            ((__DATE__)[7] - '0') * 1000 + \
            ((__DATE__)[8] - '0') * 100  + \
            ((__DATE__)[9] - '0') * 10   + \
            ((__DATE__)[10] - '0') * 1     \
        ),
        REPO_URL);
    kprintf("Commit %s (v%s.%s.%s) built on %s at %s.\n\n", COMMIT, VER_MAJOR, VER_MINOR, VER_PATCH, __DATE__, __TIME__);
}

static void bootTone()
{
    // Beep beep!
    spkr_beep(1000, 50);
    sleep(100);
    spkr_beep(1000, 50);
}

/**
 *  _  __                 _   ___     _
 * | |/ /___ _ _ _ _  ___| | | __|_ _| |_ _ _ _  _
 * | ' </ -_) '_| ' \/ -_) | | _|| ' \  _| '_| || |
 * |_|\_\___|_| |_||_\___|_| |___|_||_\__|_|  \_, |
 *                                            |__/
 */

/**
 * @brief This is the Xyris kernel entry point. This function is called directly
 * from the assembly written in boot.S located in arch/i386/boot.S. The pragma
 * allows this function to be declared without needing a former declaration.
 */
#pragma GCC diagnostic ignored "-Wmissing-declarations"
extern "C" void kernelEntry(void* boot_info, uint32_t magic)
{
    Boot::Handoff handoff;

    // Initialize the CPU
    Arch::cpuInit();
    // Initialize devices
    Arch::criticalRegion(devInit);
    // Initialize info from bootloader
    bootInit(boot_info, magic, &handoff);
    paging_init(0);
    graphics::init();
    // Print the splash screen to show we've booted into the kernel properly.
    printSplash();
    // Print some info to show we did things right
    Time::TimeDescriptor time;
    time.printDate();
    // Get the CPU vendor and model data to print
    const char* vendor = Arch::cpuGetVendor();
    const char* model = Arch::cpuGetModel();
    kprintf(DBG_INFO "%s %s\n", vendor, model);
    RS232::printf("%s\n%s\n", vendor, model);

    tasks_init();
    struct task compute, status, spinner, animation;
    tasks_new(Apps::find_primes, &compute, TASK_READY, "prime_compute");
    tasks_new(Apps::show_primes, &status, TASK_READY, "prime_display");
    tasks_new(Apps::spinner, &spinner, TASK_READY, "spinner");
    tasks_new(Apps::testAnimation, &animation, TASK_READY, "testAnimation");
    // Now that we're done make a joyful noise
    bootTone();

    // Keep the kernel task alive.
    tasks_block_current(TASK_PAUSED);
    PANIC("Kernel terminated unexpectedly!");
}
