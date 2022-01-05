/**
 * @file Interrupts.h
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Interrupt/Exception stub functions and handlers
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <Arch/i686/regs.hpp>

namespace Interrupts {

extern "C" {

// Exception stubs
void exception0();
void exception1();
void exception2();
void exception3();
void exception4();
void exception5();
void exception6();
void exception7();
void exception8();
void exception9();
void exception10();
void exception11();
void exception12();
void exception13();
void exception14();
void exception15();
void exception16();
void exception17();
void exception18();
void exception19();
void exception20();
void exception21();
void exception22();
void exception23();
void exception24();
void exception25();
void exception26();
void exception27();
void exception28();
void exception29();
void exception30();
void exception31();

// Interrupt stubs
void interrupt0();
void interrupt1();
void interrupt2();
void interrupt3();
void interrupt4();
void interrupt5();
void interrupt6();
void interrupt7();
void interrupt8();
void interrupt9();
void interrupt10();
void interrupt11();
void interrupt12();
void interrupt13();
void interrupt14();
void interrupt15();

/**
 * @brief CPU exception handler. Must be available for each exception
 * stub to be able to call.
 *
 * @param regs CPU registers structure
 */
void exceptionHandler(struct registers* regs);

/**
 * @brief CPU interrupt handler. Must be available for each interrupt
 * stub to be able to call.
 *
 * @param regs CPU registers structure
 */
void interruptHandler(struct registers* regs);

} // !extern "C"

} // !namespace Interrupts
