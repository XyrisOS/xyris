/**
 * @file tasks.hpp
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-08-29
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#ifndef PANIX_TASKS_HPP
#define PANIX_TASKS_HPP

#include <stdint.h>    // Data type definitions
#include <arch/arch.hpp>    // Architecture specific features
#include <mem/paging.hpp>

enum px_task_state {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_STOPPED,
    TASK_PAUSED
};

typedef struct px_task px_task_t;

struct px_task
{
    uintptr_t stack_top;
    uintptr_t page_dir;
    px_task_t *next_task;
    px_task_state state;
    uint64_t time_used;
};

extern px_task_t *px_current_task;

void px_tasks_init();
extern "C" void px_tasks_switch_to(px_task_t *task);
px_task_t *px_tasks_new(void (*entry)(void));
void px_tasks_schedule();
uint64_t px_tasks_get_self_time();
void px_tasks_block_current(px_task_state reason);
void px_tasks_unblock(px_task_t *task);

#endif /* PANIX_TASKS_HPP */
