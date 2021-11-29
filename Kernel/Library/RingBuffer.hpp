/**
 * @file RingBuffer.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A ring buffer is a buffer method where the same
 * memory used to contain data is reused. Inserting and removing
 * data is done in a queue-like fashion.
 * @version 0.3
 * @date 2020-09-12
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

#include <stdint.h>
#include <Library/errno.hpp>

template <typename T, size_t S>
class RingBuffer {
public:
    /**
     * @brief Initializes the circular buffer and allocates
     * the data memory.
     */
    explicit RingBuffer()
        : head(0)
        , tail(0)
        , length(0)
        , error(0)
    {
        // Default constructor
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
            error = BufferFull;
        } else {
            // Write the data at the write index
            this->data[this->head] = val;
            this->head = ((this->head + 1) % S);
            ++this->length;
        }
        // Return the status code
        return status;
    }

    /**
     * @brief Dequeues from the circular buffer and writes
     * the value to the data pointer.
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
            error = InvalidValue;
        } else {
            // Read out the data and decrement the position
            *buf = this->data[this->tail];
            this->tail = ((this->tail + 1) % S);
            --this->length;
        }
        // Return the status code
        return status;
    }

    /**
     * @brief Dequeues from the circular buffer and returns
     * the data.
     *
     * @return T Returns dequeued data.
     */
    T Dequeue()
    {
        T val = 0;
        // Check if the buffer is empty. If so, we can't dequeue
        if (IsEmpty()) {
            error = InvalidValue;
        } else {
            // Read out the data and decrement the position
            val = this->data[this->tail];
            this->tail = ((this->tail + 1) % S);
            --this->length;
        }
        // Return the status code
        return val;
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
            error = InvalidValue;
        } else {
            // Read out the data and don't decrement the position
            *buf = this->data[this->tail];
        }
        // Return the status code
        return status;
    }

    /**
     *
     * @brief Query whether the circular buffer is empty.
     *
     * @return true The buffer is empty
     * @return false The buffer is not empty
     */
    bool IsEmpty()
    {
        return this->length == 0;
    }

    /**
     * @brief Query whether the circular buffer is full.
     *
     * @return true The buffer is full
     * @return false The buffer is not full
     */
    bool IsFull()
    {
        return this->length == S;
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
        return S;
    }

    /**
     * @brief Returns the ring buffer error code.
     *
     * @return int Error code
     */
    int Error()
    {
        return error;
    }

private:
    T data[S];
    size_t head;
    size_t tail;
    size_t length;
    int error;
};
