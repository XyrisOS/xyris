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

typedef struct ring_buff {
    uint8_t* data;
    int head;
    int tail;
    int size;
    int length;
    bool external;
} ring_buff_t;

/**
 * @brief Initializes the circular buffer and allocates
 * the data memory.
 *
 * @param buff Reference circular buffer
 * @param size Maximum buffer capacity (in bytes)
 * @param data Data buffer to be used. If NULL, a buffer of
 * size will be allocated and internally managed. If a buffer
 * is provided, the caller is responsible for memory management
 * of the provided buffer.
 * @return int Returns 0 on success and -1 on error.
 */
int ring_buffer_init(ring_buff_t* buff, int size, uint8_t* data);
/**
 * @brief Destroys a circular buffer and frees all the
 * data memory.
 *
 * @param buff Reference circular buffer
 * @return int Returns 0 on success and -1 on error.
 */
int ring_buffer_destroy(ring_buff_t* buff);
/**
 * @brief Writes a byte into the circular buffer.
 *
 * @param buff Reference circular buffer
 * @param byte Byte to write to the buffer
 */
int ring_buffer_enqueue(ring_buff_t* buff, uint8_t byte);
/**
 * @brief Removes a bytes from the circular buffer and writes
 * the value of the byte to the data pointer.
 *
 * @param buff Reference circular buffer
 * @param data Pointer to uint8_t to contain the data
 * @return int Returns 0 on success and -1 on error.
 */
int ring_buffer_dequeue(ring_buff_t* buff, uint8_t* data);
/**
 * @brief Grab the latest bytes of data from the buffer without
 * removing it.
 *
 * @param buff Reference circular buffer
 * @param data Pointer to uint8_t to contain the data
 * @return int Returns 0 on success and -1 on error.
 */
int ring_buffer_peek(ring_buff_t* buff, uint8_t* data);
/**
 * @brief Query whether the circular buffer is empty.
 *
 * @param buff Reference circular buffer
 * @return true The buffer is empty
 * @return false The buffer is not empty
 */
bool ring_buffer_is_empty(ring_buff_t* buff);
/**
 * @brief Query whether the circular buffer is full.
 *
 * @param buff Reference circular buffer
 * @return true The buffer is full
 * @return false The buffer is not full
 */
bool ring_buffer_is_full(ring_buff_t* buff);
/**
 * @brief Returns the number of items (bytes) in the buffer.
 *
 * @param buff Reference circular buffer
 * @return int Number of bytes available for reading.
 */
int ring_buffer_length(ring_buff_t* buff);
/**
 * @brief Returns the buffer capacity (in number of bytes).
 *
 * @param buff Reference circular buffer
 * @return int Buffer capacity in bytes
 */
int ring_buffer_size(ring_buff_t* buff);
