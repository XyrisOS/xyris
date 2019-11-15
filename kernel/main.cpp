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
// Intel i386 architecture
#include <arch/i386/gdt.hpp>
#include <arch/i386/idt.hpp>
#include <arch/i386/isr.hpp>
// Generic devices
#include <devices/smbios/smbios.hpp>

void px_kernel_print_splash();

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
 * assembly written in boot.S located in kernel/arch/i386/boot.S.
 * 
 */
extern "C" void px_kernel_main(const void* multiboot_structure, uint32_t multiboot_magic) {
    // Print the splash screen to show we've booted into the kernel properly.
    px_kernel_print_splash();
    kprintSetColor(Blue, Black);
    px_gdt_install() ? px_print_debug("Loaded GDT.", Success) : panic("Unable to install the GDT!");
    char* smbios_addr = px_get_smbios_addr();
    px_print_debug("Installing ISR...", Info);
    px_isr_install();
    px_print_debug("We got past the ISQ install", Info);
    __asm__ __volatile__("int $0x2");
    __asm__ __volatile__("int $0x3");
}

void px_kernel_print_splash() {
    clearScreen();
    kprintSetColor(Yellow, Black);
    kprint("Welcome to Panix\n");
    kprint("Developed by graduates and undergraduates of Cedarville University.\n");
    kprint("Copyright Keeton Feavel et al (c) 2019. All rights reserved.\n\n");
    kprintSetColor(White, Black);
}