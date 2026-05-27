/**
 * @file pipes.hpp
 * @author Amilie Baker (amiliefn@gmail.com)
 * @brief Piping infrastructure for IPC.
 * @version 0.1
 * @date 2026-05-26
 *
 * @copyright Copyright the Xyris Contributors (c) 2026
 *
 */

#include <Locking/Semaphore.hpp>
#include <Locking/Mutex.hpp>

/*
    Semaphore want's values like this yah.
*/
struct pipe_t{
    uint8_t buffer[4096];
    size_t read_pos;
    size_t write_pos;
    size_t count;
    Semaphore data_available;
    Semaphore space_available;
    Mutex lock;
    int readers;
    int writers;

    pipe_t()
        : read_pos(0)
        , write_pos(0)
        , count(0)
        , data_available(0, false, "pipe_data")
        , space_available(4096, false, "pipe_space")
        , readers(1)
        , writers(1)
    {}
};

pipe_t* create_pipe();
int write_pipe(pipe_t* pipe, const void* data, size_t size);
int read_pipe(pipe_t* pipe, void* buffer, size_t size);
void close_pipe(pipe_t* pipe, bool is_write_end);