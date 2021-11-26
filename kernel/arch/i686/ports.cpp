/**
 * @file ports.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief CPU port access functions
 * @version 0.3
 * @date 2020-06-30
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#include <arch/i686/ports.hpp>

uint8_t readByte(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void writeByte(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

void writeByteSlow(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (port));
}

uint16_t readWord(uint16_t port) {
    uint16_t result;
    asm volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void writeWord(uint16_t port, uint16_t data) {
    asm volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

uint32_t readLong(uint16_t port) {
    uint32_t result;
    asm volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void writeLong(uint16_t port, uint32_t data) {
    asm volatile("outl %0, %1" : : "a"(data), "Nd" (port));
}
