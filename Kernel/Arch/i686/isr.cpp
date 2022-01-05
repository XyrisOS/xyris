/**
 * @file isr.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Xyris Contributors (c) 2019
 *
 */
#include <Arch/Arch.hpp>
#include <Arch/i686/idt.hpp>
#include <Arch/i686/isr.hpp>
#include <Arch/i686/Assembly/Interrupts.h>
#include <Panic.hpp>

namespace Interrupts {

// Interrupt handler function pointers
InterruptHandler_t interruptHandlers[256];

extern "C" {

/**
 * @brief Hardware exception handler. Called by each exception handler stub.
 *
 * @param regs CPU registers structure. Includes interrupt number.
 */
void exceptionHandler(struct registers* regs)
{
    panic(regs);
}

/**
 * @brief Hardware interrupt handler. Called by each interrupt handler stub.
 *
 * @param regs CPU registers structure. Includes interrupt number.
 */
void interruptHandler(struct registers* regs)
{
    // After every interrupt we need to send an EOI to the PICs or it won't send another
    if (regs->int_num >= 0x28) {
        // Respond to secondard PIC
        writeByte(0xA0, 0x20);
    }

    // Respond to primary PIC
    writeByte(0x20, 0x20);

    if (interruptHandlers[regs->int_num]) {
        InterruptHandler_t handler = interruptHandlers[regs->int_num];
        handler(regs);
    }
}

} // !extern "C"

void (*exceptionHandlerStubs[ARCH_EXCEPTION_NUM])(void) = {
    exception0,  exception1,  exception2,  exception3,  exception4,  exception5,  exception6,  exception7,
    exception8,  exception9,  exception10, exception11, exception12, exception13, exception14, exception15,
    exception16, exception17, exception18, exception19, exception20, exception21, exception22, exception23,
    exception24, exception25, exception26, exception27, exception28, exception29, exception30, exception31
};

void (*interruptHandlerStubs[ARCH_INTERRUPT_NUM])(void) = {
    interrupt0, interrupt1, interrupt2,  interrupt3,  interrupt4,  interrupt5,  interrupt6,  interrupt7,
    interrupt8, interrupt9, interrupt10, interrupt11, interrupt12, interrupt13, interrupt14, interrupt15
};

void init()
{
    // Set all of the gate addresses
    for (int exception = 0; exception < 32; exception++) {
        IDT::setGate(exception, (uint32_t)exceptionHandlerStubs[exception]);
    }

    // Remap the programmable interrupt controller
    writeByte(0x20, 0x11);
    writeByte(0xA0, 0x11);
    writeByte(0x21, 0x20);
    writeByte(0xA1, 0x28);
    writeByte(0x21, 0x04);
    writeByte(0xA1, 0x02);
    writeByte(0x21, 0x01);
    writeByte(0xA1, 0x01);
    writeByte(0x21, 0x0);
    writeByte(0xA1, 0x0);

    // Install the interrupt requests
    for (int interrupt = 0; interrupt < ARCH_INTERRUPT_NUM; interrupt++) {
        IDT::setGate(32 + interrupt, (uint32_t)interruptHandlerStubs[interrupt]);
    }

    // Load the IDT now that we've registered all of our IDT, IRQ, and ISR addresses
    IDT::init();
}

void registerHandler(uint8_t interrupt, InterruptHandler_t handler)
{
    interruptHandlers[interrupt] = handler;
}

} // !namespace Interrupts
