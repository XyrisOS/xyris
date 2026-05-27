/**
 * @file messages.hpp
 * @author Amilie Baker (amiliefn@gmail.com)
 * @brief Messaging infrastructure for IPC.
 * @version 0.1
 * @date 2026-05-27 
 *
 * @copyright Copyright the Xyris Contributors (c) 2026
 * @ref https://wiki.osdev.org/Message_Passing
 * @ref https://stackoverflow.com/questions/25074741/how-do-i-store-and-get-a-queue-of-structure
 *
 */
#include <Locking/Semaphore.hpp>
#include <Locking/Mutex.hpp>
#include <Locking/RAII.hpp>

struct msg_t{
    uint8_t sender_id;
    uint8_t type_id;
    uint8_t payload[64];

    msg_t()
        : sender_id(0)
        , type_id(0)
        , payload{}
    {}
};

struct msg_queue_t{
    msg_t buffer[64];
    size_t read_pos;
    size_t write_pos;
    size_t count;
    Semaphore data_available;
    Semaphore space_available;
    Mutex lock;
    int readers;
    int writers;

    msg_queue_t()
    : read_pos(0)
    , write_pos(0)
    , count(0)
    , data_available(0, false, "msg_data")
    , space_available(64, false, "msg_space")
    , readers(1)
    , writers(1)
{}
};

msg_queue_t* create_msg();
int write_msg(msg_queue_t* queue, msg_t* msg);
int read_msg(msg_queue_t* queue, msg_t* msg);
void close_msg(msg_queue_t* queue, bool is_write_end);