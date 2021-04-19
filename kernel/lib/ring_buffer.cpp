/**
 * @file ring_buffer.cpp
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

#include <lib/ring_buffer.hpp>
#include <mem/heap.hpp>
#include <lib/errno.h>

int px_ring_buffer_init(px_ring_buff_t* buff, int size, uint8_t* data) {
    int status = 0;
    // Check if the pointer is valid
    if (buff != NULL) {
        // Set the struct variables (set locations and size)
        buff->size = size;
        buff->head = 0;
        buff->tail = 0;
        buff->length = 0;
        // Should we use provided buffer?
        if (data != NULL) {
            buff->data = data;
            buff->external = true;
        } else {
            // Allocate the memory for our buffer
            buff->data = (uint8_t *)malloc(sizeof(uint8_t) * size);
            buff->external = false;
            // Did we actually allocate the data?
            if (buff->data == NULL) {
                status = -1;
                errno = ENOMEM;
            }
        }
    } else {
        status = -1;
        errno = EINVAL;
    }
    // Return our status code
    return status;
}

int px_ring_buffer_destroy(px_ring_buff_t* buff) {
    int status = 0;
    if (buff != NULL) {
        // Free the data buffer if self managed
        if (buff->external == false) {
            // Free our data buffer and set the struct to null
            free(buff->data);
        }
        buff = NULL;
    } else {
        status = -1;
        errno = EINVAL;
    }
    return status;
}

int px_ring_buffer_enqueue(px_ring_buff_t* buff, uint8_t byte) {
    int status = 0;
    if (buff != NULL) {
        // Check if the buffer is full. If so, we can't enqueue.
        if (px_ring_buffer_is_full(buff)) {
            status = -1;
            errno = ENOBUFS;
        } else {
            // Write the data at the write index
            buff->data[buff->head] = byte;
            buff->head = ((buff->head + 1) % buff->size);
            ++buff->length;
        }
    } else {
        status = -1;
        errno = EINVAL;
    }
    // Return the status code
    return status;
}

int px_ring_buffer_dequeue(px_ring_buff_t* buff, uint8_t* data) {
    int status = 0;
    if (buff != NULL) {
        // Check if the buffer is empty. If so, we can't dequeue
        if (px_ring_buffer_is_empty(buff)) {
            status = -1;
            errno = EINVAL;
        } else {
            // Read out the data and decrement the position
            *data = buff->data[buff->tail];
            buff->tail = ((buff->tail + 1) % buff->size);
            --buff->length;
        }
    } else {
        status = -1;
        errno = EINVAL;
    }
    // Return the status code
    return status;
}

int px_ring_buffer_peek(px_ring_buff_t* buff, uint8_t* data) {
    int status = 0;
    if (buff != NULL) {
        // Check if the buffer is empty. If so, we can't dequeue
        if (px_ring_buffer_is_empty(buff)) {
            status = -1;
            errno = EINVAL;
        } else {
            // Read out the data and don't decrement the position
            *data = buff->data[buff->tail];
        }
    } else {
        status = -1;
        errno = EINVAL;
    }
    // Return the status code
    return status;
}

bool px_ring_buffer_is_empty(px_ring_buff_t* buff) {
    if (buff != NULL) {
        return (buff->length == 0);
    } else {
        errno = EINVAL;
        return false;
    }
}

bool px_ring_buffer_is_full(px_ring_buff_t* buff) {
    if (buff != NULL) {
        return (buff->length == buff->size);
    } else {
        errno = EINVAL;
        return false;
    }
}

int px_ring_buffer_length(px_ring_buff_t* buff) {
    if (buff != NULL) {
        return buff->length;
    } else {
        errno = EINVAL;
        return false;
    }
}

int px_ring_buffer_size(px_ring_buff_t* buff) {
    if (buff != NULL) {
        return buff->size;
    } else {
        errno = EINVAL;
        return false;
    }
}
