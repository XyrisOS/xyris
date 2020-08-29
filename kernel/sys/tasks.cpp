/**
 * @file tasks.cpp
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-08-29
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */
#include <sys/tasks.hpp>
#include <mem/heap.hpp>
#include <sys/panic.hpp>
#include <stdint.h>    // Data type definitions

px_task_t *px_current_task = NULL;

void px_tasks_init()
{
    px_task_t *this_task = (px_task_t*)malloc(sizeof(px_task_t));
    if (this_task == NULL) PANIC("Unable to allocate memory for new task struct.\n");
    *this_task = {
        .stack_top = 0,
        .page_dir = px_get_phys_page_dir(),
        .next_task = NULL,
        .state = TASK_RUNNING
    };
    px_current_task = this_task;
}

// TODO: code here
