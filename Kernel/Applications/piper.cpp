/**
 * @file piper.cpp
 * @author Amilie Baker (amiliefn@gmail.com)
 * @brief A piping test / solution to ensure IPC works.
 * @version 0.2
 * @date 2026-05-26
 *
 * @copyright Copyright the Xyris Contributors (c) 2026
 *
 */

#include <IPC/pipes.hpp>
#include <IPC/messages.hpp>
#include <Devices/Graphics/console.hpp>
#include <Applications/piper.hpp>

namespace Apps {

    void piper()
    {
        pipe_t* p = create_pipe();
        
        const char* msg = "Hello pipes!";
        size_t len = 12;
        
        write_pipe(p, msg, len);
        
        char result[12];
        read_pipe(p, result, len);
        
        bool passed = true;
        for (size_t i = 0; i < len; i++){
            if (result[i] != msg[i]){
                passed = false;
            }
        }

        if(passed){
            Console::printf("Passed Pipe Test!\n");
        }else{
            Console::printf("Failed Pipe Test!\n");
        }
                
        close_pipe(p, true);
        close_pipe(p, false);

        msg_queue_t* q = create_msg();

        msg_t send;
        send.sender_id = 1;
        send.type_id = 42;
        send.payload[0] = 'H';
        send.payload[1] = 'i';

        write_msg(q, &send);
        msg_t recv;
        read_msg(q, &recv);

        bool msg_passed = true;
        if (recv.sender_id != send.sender_id) msg_passed = false;
        if (recv.type_id != send.type_id) msg_passed = false;
        if (recv.payload[0] != send.payload[0]) msg_passed = false;
        if (recv.payload[1] != send.payload[1]) msg_passed = false;

        if (msg_passed) {
            Console::printf("Passed Message Test!\n");
        } else {
            Console::printf("Failed Message Test!\n");
        }

        close_msg(q, true);
        close_msg(q, false);
    }
}
