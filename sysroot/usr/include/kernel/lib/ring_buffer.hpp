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
#include <lib/errno.h>

template <typename T> class RingBuffer {
public:
    /**
     * @brief Initializes the circular buffer and allocates
     * the data memory.
     *
     * @param size Maximum buffer capacity (in bytes)
     * @param buf Data buffer to be used. If NULL, a buffer of
     * size will be allocated and internally managed. If a buffer
     * is provided, the caller is responsible for memory management
     * of the provided buffer.
     * @return int Returns 0 on success and -1 on error.
     */
    explicit RingBuffer(int size, T* buf)
        : head(0)
        , tail(0)
        , length(0)
        , capacity(size)
    {
        this->data = buf;
    }
    /**
     * @brief Writes a byte into the circular buffer.
     *
     * @param val Data to write to the buffer
     */
    int Enqueue(T val)
    {
        int status = 0;
        // Check if the buffer is full. If so, we can't enqueue.
        if (IsFull()) {
            status = -1;
            errno = ENOBUFS;
        } else {
            // Write the data at the write index
            this->data[this->head] = val;
            this->head = ((this->head + 1) % this->capacity);
            ++this->length;
        }
        // Return the status code
        return status;
    }
    /**
     * @brief Removes a bytes from the circular buffer and writes
     * the value of the byte to the data pointer.
     *
     * @param buf Buffer to contain the data
     * @return int Returns 0 on success and -1 on error.
     */
    int Dequeue(T* buf)
    {
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
    /**
     * @brief Grab the latest bytes of data from the buffer without
     * removing it.
     *
     * @param buf Buffer to contain the data
     * @return int Returns 0 on success and -1 on error.
     */
    int Peek(T* buf)
    {
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
    /**
     * @brief Query whether the circular buffer is empty.
     *
     * @return true The buffer is empty
     * @return false The buffer is not empty
     */
    bool IsEmpty()
    {
        return (this->length == 0);
    }
    /**
     * @brief Query whether the circular buffer is full.
     *
     * @return true The buffer is full
     * @return false The buffer is not full
     */
    bool IsFull()
    {
        return (this->length == this->capacity);
    }
    /**
     * @brief Returns the number of items (bytes) in the buffer.
     *
     * @return int Number of bytes available for reading.
     */
    int Length()
    {
        return this->length;
    }
    /**
     * @brief Returns the buffer capacity (in number of bytes).
     *
     * @return int Buffer capacity in bytes
     */
    int Capacity()
    {
        return this->capacity;
    }

private:
    T* data;
    int head;
    int tail;
    int length;
    int capacity;
};
