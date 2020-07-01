/**
 * @file Port.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Ports header file. Provides inline functions
 * for getting and setting values at different ports.
 * An I/O port is usually used as a technical term for 
 * a specific address on the x86's IO bus. This bus 
 * provides communication with devices in a fixed 
 * order and size, and was used as an alternative 
 * to memory access. (https://wiki.osdev.org/I/O_Ports)
 * @version 0.3
 * @date 2019-09-26
 * 
 * @copyright Copyright Keeton Feavel (c) 2019
 * 
 */
#ifndef PANIX_PORT_HPP
#define PANIX_PORT_HPP

#include <sys/panix.hpp>

/**
 * @brief Reads a byte (8 bits) off the CPU bus at a given port address
 * 
 * @param port Port address
 * @return uint8_t Returned data byte
 */
uint8_t px_read_byte(uint16_t port);
/**
 * @brief Writes a byte (8 bits) to the CPU bus at a given port address
 * 
 * @param port Port address
 * @param data Byte to be written to the port
 */
void px_write_byte(uint16_t port, uint8_t data);
/**
 * @brief Writes a byte (8 bits) slowly to the CPU bus at a given port address
 * 
 * @param port Port address
 * @param data Byte to be written to the port
 */
void px_write_byte_slow(uint16_t port, uint8_t data);
/**
 * @brief Reads a word (16 bits) off the CPU bus at a given port address
 * 
 * @param port Port address
 * @return uint8_t Returned data word
 */
uint16_t px_read_word(uint16_t port);
/**
 * @brief Writes a word (16 bits) to the CPU bus at a given port address
 * 
 * @param port Port address
 * @param data Word to be written to the port
 */
void px_write_word(uint16_t port, uint16_t data);
/**
 * @brief Reads a long (32 bits) off the CPU bus at a given port address
 * 
 * @param port Port address
 * @return uint8_t Returned data long
 */
uint32_t px_read_long(uint16_t port);
/**
 * @brief Writes a long (32 bits) to the CPU bus at a given port address
 * 
 * @param port Port address
 * @param data Long to be written to the port
 */
void px_write_long(uint16_t port, uint32_t data);

#endif /* PANIX_PORT_HPP */