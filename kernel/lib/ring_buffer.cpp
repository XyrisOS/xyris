/**
 * @file ring_buffer.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A circular buffer is a buffer method where the same
 * memory used to contain data is reused. Inserting and removing
 * data is done in a queue-like fashion.
 * @version 0.3
 * @date 2020-09-12
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#include <lib/ring_buffer.hpp>
#include <mem/heap.hpp>

int px_ring_buffer_init(px_ring_buff_t* buff, int size) {
    // Set the struct variables (set locations and size)
    buff->size = size;
    buff->head = 0;
    buff->tail = 0;
    buff->length = 0;
    // Allocate the memory for our buffer
    buff->data = (uint8_t *)malloc(sizeof(uint8_t) * size);
}

int px_ring_buffer_destroy(px_ring_buff_t* buff)
{
    // Free our data buffer and set the struct to null
    free(buff->data);
    buff = NULL;
}

int px_ring_buffer_enqueue(px_ring_buff_t* buff, uint8_t byte) {
    int status = 0;
    // Check if the buffer is full. If so, we can't enqueue.
    if (px_ring_buffer_is_full(buff)) {
        status = -1;
    } else {
        // Write the data at the write index
        buff->data[buff->head] = byte;
        buff->head = ((buff->head + 1) & (buff->size - 1));
        ++buff->length;
    }
    // Return the status code
    return status;
}

int px_ring_buffer_dequeue(px_ring_buff_t* buff, uint8_t* data) {
    int status = 0;
    // Check if the buffer is empty. If so, we can't dequeue
    if (px_ring_buffer_is_empty(buff)) {
        status = -1;
    } else {
        // Read out the data and decrement the position
        *data = buff->data[buff->tail];
        buff->tail = ((buff->tail + 1) & (buff->size - 1));
        --buff->length;
    }
    // Return the status code
    return status;
}

int px_ring_buffer_peek(px_ring_buff_t* buff, uint8_t* data) {
    int status = 0;
    // Check if the buffer is empty. If so, we can't dequeue
    if (px_ring_buffer_is_empty(buff)) {
        status = -1;
    } else {
        // Read out the data and don't decrement the position
        *data = buff->data[buff->tail];
    }
    // Return the status code
    return status;
}

bool px_ring_buffer_is_empty(px_ring_buff_t* buff) {
    return (buff->length == 0);
}

bool px_ring_buffer_is_full(px_ring_buff_t* buff) {
    return (buff->length == buff->size);
}

int px_ring_buffer_length(px_ring_buff_t* buff) {
    return buff->length;
}

int px_ring_buffer_size(px_ring_buff_t* buff) {
    return buff->size;
}
