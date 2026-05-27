/**
 * @file pipes.cpp
 * @author Amilie Baker (amiliefn@gmail.com)
 * @brief Piping infrastructure for IPC.
 * @version 0.1
 * @date 2026-05-26
 *
 * @copyright Copyright the Xyris Contributors (c) 2026
 * @ref https://github.com/mit-pdos/xv6-public/blob/master/pipe.c
 * @ref https://www.usna.edu/Users/cs/wcbrown/courses/IC221/classes/L13/Class.html
 * @ref https://wiki.osdev.org/Message_Passing
 * @ref https://github.com/mit-pdos/xv6-public/blob/master/pipe.c
 *
 */

#include <IPC/pipes.hpp>
#include <Locking/Semaphore.hpp>
#include <Locking/Mutex.hpp>
#include <Locking/RAII.hpp>

/* 
    For Mutex locking in write_pipe.
*/
Mutex mutex_pipe("pipe");
inline void* operator new(size_t, void* ptr) { return ptr; }

/*
    This shit about to be MESSY but it's gotta be done.
    Blocked the compiler warnings as it wouldn't compile. MANUALLY CONFIRMED IT IS SAFE THOUGH.
*/
pipe_t* create_pipe(){
    void* mem = operator new(sizeof(pipe_t));
    if(mem != NULL){
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-possible-null-argument"
        return new (mem) pipe_t;
#pragma GCC diagnostic pop
    }
    return 0;
}

/*
    Writing info to the pipes, ensuring that different processes cannot conflict.
*/
int write_pipe(pipe_t* pipe, const void* data, size_t size){
    for (size_t i = 0; i < size; i++){
        pipe->space_available.wait();
        if(pipe->readers == 0){
            return -1;
        }else{
            RAIIMutex lock(mutex_pipe);

            /*
                Casting so we can actually iterate on it.
                (This weather is horrible. FML.)

                - Ami, 26/05/26
            */
            pipe->buffer[pipe->write_pos] = ((uint8_t*)data)[i];

            pipe->write_pos = (pipe->write_pos + 1) % 4096;
            pipe->count++;
        }
            pipe->data_available.post();
    }
    return size;
}

/*
    Literally just an inversion of write_pipe.
*/
int read_pipe(pipe_t *pipe, void* data, size_t size){
    for (size_t i = 0; i < size; i++){
        pipe->space_available.wait();

        if(pipe->writers == 0 && pipe->count==0){
            return 0;
        }else{
            RAIIMutex lock(mutex_pipe);
            ((uint8_t*)data)[i] = pipe->buffer[pipe->read_pos];

            pipe->read_pos = (pipe->read_pos + 1) % 4096;
            pipe->count--;
        }
            pipe->space_available.post();
    }
    return size;
}

/*
    Checks for complete freeing and does cleanup.
*/
void close_pipe(pipe_t *pipe, bool is_write_end){
    if(is_write_end == 1){
        pipe->writers--;
        if(pipe->writers == 0){
            pipe->data_available.post();
        }
    }else{
        pipe->readers--;
        if(pipe->readers == 0){
            pipe->space_available.post();
        }
    }

    if(pipe->readers == 0 && pipe->writers == 0){
        delete pipe;
    }
}