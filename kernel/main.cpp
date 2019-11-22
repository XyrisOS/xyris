/**
 * @file main.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The entry point into the Panix kernel. Everything is loaded from here.
 * @version 0.1
 * @date 2019-11-14
 * 
 * @copyright Copyright Keeton Feavel et al (c) 2019
 * 
 */
// System library functions
#include <sys/sys.hpp>
#include <mem/paging.hpp>
// Multiboot Structure
#include <sys/multiboot.hpp>
// Intel i386 architecture
#include <arch/x86/gdt.hpp>
#include <arch/x86/idt.hpp>
#include <arch/x86/isr.hpp>
#include <arch/x86/timer.hpp>
// Generic devices
#include <devices/smbios/smbios.hpp>
#include <devices/kbd/kbd.hpp>
#include <devices/rtc/rtc.hpp>

void px_kernel_print_splash();

/**
 * @brief Global constructor called from the boot assembly
 * OSDev Wiki takes a different approach to this and does
 * all of this in assembly. You can see that for yourself
 * in the Meaty Skeleton tutorial in the crti.S section.
 */
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void px_call_constructors() {
    for (constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/x86/boot.S.
 * @todo Figure out how to use the multiboot header passed in to set up virtual memory
 * and other features.
 */
extern "C" void px_kernel_main(uint32_t mb_magic, const multiboot_info_t* mb_struct, uintptr_t vmem) {
    // Print the splash screen to show we've booted into the kernel properly.
    px_kernel_print_splash();
    px_tty_set_color(Blue, Black);
    // Install the GDT
    px_interrupts_disable();
    px_gdt_install() ? px_print_debug("Loaded GDT.", Success) : panic("Unable to install the GDT!");
    /**
     * @todo Make success and fail conditions for all of these and fix SMBIOS
     */
    //char* smbios_addr = px_get_smbios_addr();
    px_isr_install();           // Interrupt Service Requests
    px_kbd_init();              // Keyboard
    px_rtc_init();              // Real Time Clock
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
    px_paging_init();
    px_interrupts_enable();     // Enable interrupts
    // Print some info to show we did things right
    px_rtc_print();
    px_print_debug("Done.", Success);
    //uint32_t *ptr = (uint32_t*)0xA0000000;
    //uint32_t do_page_fault = *ptr;
    while (true) {
        // Keep the kernel alive.
    }
    panic("Yikes!\nKernel terminated unexpectedly.");
}

void px_kernel_print_splash() {
    px_clear_tty();
    px_tty_set_color(Yellow, Black);
    kprint("Welcome to Panix\n");
    kprint("Developed by graduates and undergraduates of Cedarville University.\n");
    kprint("Copyright Keeton Feavel et al (c) 2019. All rights reserved.\n\n");
    px_tty_set_color(LightCyan, Black);
    kprint("Gloria in te domine, Gloria exultate\n\n");
    px_tty_set_color(White, Black);
}