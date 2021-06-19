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

RingBuffer::RingBuffer(int size, uint8_t* buf)
    : head(0)
    , tail(0)
    , length(0)
    , capacity(size)
{
    // Should we use provided buffer?
    if (buf != NULL) {
        this->data = buf;
        this->external = true;
    } else {
        // Allocate the memory for our buffer
        this->data = (uint8_t *)malloc(sizeof(uint8_t) * capacity);
        this->external = false;
        // Did we actually allocate the data?
        if (this->data == NULL) {
            errno = ENOMEM;
        }
    }
}

RingBuffer::~RingBuffer() {
    // Free the data buffer if self managed
    if (this->external == false) {
        // Free our data buffer and set the struct to null
        free(this->data);
    }
}

int RingBuffer::Enqueue(uint8_t byte) {
    int status = 0;
    // Check if the buffer is full. If so, we can't enqueue.
    if (IsFull()) {
        status = -1;
        errno = ENOBUFS;
    } else {
        // Write the data at the write index
        this->data[this->head] = byte;
        this->head = ((this->head + 1) % this->capacity);
        ++this->length;
    }
    // Return the status code
    return status;
}

int RingBuffer::Dequeue(uint8_t* buf) {
    int status = 0;
    // Check if the buffer is empty. If so, we can't dequeue
    if (IsEmpty()) {
        status = -1;
        errno = EINVAL;
    } else {
        // Read out the data and decrement the position
        *buf = this->data[this->tail];
        this->tail = ((this->tail + 1) % this->capacity);
        --this->length;
    }
    // Return the status code
    return status;
}

int RingBuffer::Peek(uint8_t* buf) {
    int status = 0;
    // Check if the buffer is empty. If so, we can't dequeue
    if (IsEmpty()) {
        status = -1;
        errno = EINVAL;
    } else {
        // Read out the data and don't decrement the position
        *buf = this->data[this->tail];
    }
    // Return the status code
    return status;
}

bool RingBuffer::IsEmpty() {
    return (this->length == 0);
}

bool RingBuffer::IsFull() {
    return (this->length == this->capacity);
}

int RingBuffer::Length() {
    return this->length;
}

int RingBuffer::Capacity() {
    return this->capacity;
}
