/**
 * @file entry.cpp
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
#include <lib/time.hpp>
#include <stdint.h>
#include <sys/tasks.hpp>
#include <sys/Panic.hpp>
// Bootloader
#include <Bootloader/Handoff.hpp>
// Architecture specific code
#include <Arch/Arch.hpp>
// Memory management & paging
#include <mem/paging.hpp>
// Generic devices
#include <dev/graphics/graphics.hpp>
#include <dev/graphics/console.hpp>
#include <dev/rtc/rtc.hpp>
#include <dev/serial/rs232.hpp>
#include <dev/spkr/spkr.hpp>
// Apps
#include <Applications/primes.hpp>
#include <Applications/spinner.hpp>
// Meta
#include <meta/defines.hpp>

static void printSplash();
static void bootTone();

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
    Console::printf(
        "\033[93m"
        "Xyris %s\n"
        "Copyright the Xyris Contributors (c) %i. All rights reserved.\n"
        "Kernel source available at %s.\n"
        "\033[0m",
        VER_NAME,
        BUILD_DATE,
        REPO_URL);
    Console::printf("Commit %s (v%s.%s.%s) built on %s at %s.\n\n", COMMIT, VER_MAJOR, VER_MINOR, VER_PATCH, __DATE__, __TIME__);
}

static void bootTone()
{
    // Beep beep!
    spkr_beep(1000, 50);
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
 * from the assembly written in boot.S located in Arch/i686/boot.S. The pragma
 * allows this function to be declared without needing a former declaration.
 */
extern "C" void kernelEntry(void* info, uint32_t magic)
{
    // Initialize the CPU
    Arch::CPU::init();
    // Initialize devices
    Arch::CPU::criticalRegion(devInit);
    // Initialize info from bootloader
    Boot::Handoff handoff(info, magic);
    Memory::init(handoff.MemoryMap());
    Graphics::init(handoff.FramebufferInfo());
    // Print the splash screen to show we've booted into the kernel properly.
    printSplash();
    // Print some info to show we did things right
    Time::TimeDescriptor time;
    Console::printf(DBG_INFO "UTC: %i/%i/%i %i:%i\n",
        time.getMonth(),
        time.getDay(),
        time.getYear(),
        time.getHour(),
        time.getMinutes());
    // Get the CPU vendor and model data to print
    const char* vendor = Arch::CPU::vendor();
    const char* model = Arch::CPU::model();
    Console::printf(DBG_INFO "%s %s\n", vendor, model);
    RS232::printf("%s\n%s\n", vendor, model);

    tasks_init();
    struct task compute, status, spinner;
    tasks_new(Apps::find_primes, &compute, TASK_READY, "prime_compute");
    tasks_new(Apps::show_primes, &status, TASK_READY, "prime_display");
    tasks_new(Apps::spinner, &spinner, TASK_READY, "spinner");
    // Now that we're done make a joyful noise
    bootTone();

    // Keep the kernel task alive.
    tasks_block_current(TASK_PAUSED);
    panic("Kernel terminated unexpectedly!");
}
