/**
 * @file ring_buffer.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A ring buffer is a buffer method where the same
 * memory used to contain data is reused. Inserting and removing
 * data is done in a queue-like fashion.
 * @version 0.3
 * @date 2020-09-12
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */
#pragma once

#include <stdint.h>

class RingBuffer {
public:
    /**
     * @brief Initializes the circular buffer and allocates
     * the data memory.
     *
     * @param size Maximum buffer capacity (in bytes)
     * @param data Data buffer to be used. If NULL, a buffer of
     * size will be allocated and internally managed. If a buffer
     * is provided, the caller is responsible for memory management
     * of the provided buffer.
     * @return int Returns 0 on success and -1 on error.
     */
    RingBuffer(int size, uint8_t* buf);
    /**
     * @brief Destroys a circular buffer and frees all the
     * data memory.
     */
    ~RingBuffer();
    /**
     * @brief Writes a byte into the circular buffer.
     *
     * @param byte Byte to write to the buffer
     */
    int Enqueue(uint8_t byte);
    /**
     * @brief Removes a bytes from the circular buffer and writes
     * the value of the byte to the data pointer.
     *
     * @param buf Pointer to uint8_t to contain the data
     * @return int Returns 0 on success and -1 on error.
     */
    int Dequeue(uint8_t* buf);
    /**
     * @brief Grab the latest bytes of data from the buffer without
     * removing it.
     *
     * @param buf Pointer to uint8_t to contain the data
     * @return int Returns 0 on success and -1 on error.
     */
    int Peek(uint8_t* buf);
    /**
     * @brief Query whether the circular buffer is empty.
     *
     * @return true The buffer is empty
     * @return false The buffer is not empty
     */
    bool IsEmpty();
    /**
     * @brief Query whether the circular buffer is full.
     *
     * @return true The buffer is full
     * @return false The buffer is not full
     */
    bool IsFull();
    /**
     * @brief Returns the number of items (bytes) in the buffer.
     *
     * @return int Number of bytes available for reading.
     */
    int Length();
    /**
     * @brief Returns the buffer capacity (in number of bytes).
     *
     * @return int Buffer capacity in bytes
     */
    int Capacity();

private:
    uint8_t* data;
    int head;
    int tail;
    int length;
    int capacity;
    bool external;
};
