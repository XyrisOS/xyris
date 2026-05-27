/**
 * @file messages.cpp
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

#include <IPC/messages.hpp>
#include <Locking/Semaphore.hpp>
#include <Locking/Mutex.hpp>
#include <Locking/RAII.hpp>

/*
    This code is BASICALLY the same as pipes.ccp - I'm using the same jank workarounds.
    Good luck percieving it.
    - Ami, 27/05/26
*/
Mutex mutex_msg("msg");
inline void* operator new(size_t, void* ptr) { return ptr; }

msg_queue_t* create_msg(){
    void* mem = operator new(sizeof(msg_queue_t));
    if(mem != NULL){
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-possible-null-argument"
        return new (mem) msg_queue_t;
#pragma GCC diagnostic pop
    }
    return 0;
}

/*
    Main change here is changing of uint to just *msg, and also the reduction in buffer count to 64.
    We also do not iterate thru every byte now.
    - Ami, 27/05/26
*/
int write_msg(msg_queue_t* queue, msg_t* msg){  
    queue->space_available.wait();
    if(queue->readers == 0){
        return -1;
    }else{
        RAIIMutex lock(mutex_msg);
        queue->buffer[queue->write_pos] = *msg;

        queue->write_pos = (queue->write_pos + 1) % 64;
        queue->count++;
        queue->data_available.post();
    }
    return 1;
}

int read_msg(msg_queue_t* queue, msg_t* msg){
    queue->data_available.wait();

    if(queue->writers == 0 && queue->count==0){
        return 0;
    }else{
        RAIIMutex lock(mutex_msg);
        *msg = queue->buffer[queue->read_pos];

        queue->read_pos = (queue->read_pos + 1) % 64;
        queue->count--;
    }
    queue->space_available.post();
    return 1;
}

void close_msg(msg_queue_t* queue, bool is_write_end){
    if(is_write_end == 1){
        queue->writers--;
        if(queue->writers == 0){
            queue->data_available.post();
        }
    }else{
        queue->readers--;
        if(queue->readers == 0){
            queue->space_available.post();
        }
    }

    if(queue->readers == 0 && queue->writers == 0){
        delete queue;
    }
}