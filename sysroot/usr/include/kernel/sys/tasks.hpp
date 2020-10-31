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
#pragma once

#include <stdint.h>         // Data type definitions
#include <arch/arch.hpp>    // Architecture specific features
#include <mem/paging.hpp>

#define TIME_SLICE_SIZE (1 * 1000 * 1000ULL)

enum px_task_state
{
    TASK_RUNNING  = 0,
    TASK_READY    = 1,
    TASK_SLEEPING,
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
    uint64_t wakeup_time;
};

extern px_task_t *px_current_task;

typedef struct px_tasklist
{
    px_task_t *head;
    px_task_t *tail;
} px_tasklist_t;

#define MAX_TASKS_QUEUED 8
typedef struct px_tasks_sync
{
    px_task_t* possessor;
    uint16_t queued_head;
    uint16_t queued_tail;
    px_task_t *queued[MAX_TASKS_QUEUED];
} px_tasks_sync_t;

#define TASKS_SYNC_INIT(sync) \
    (sync)->possessor = NULL; \
    (sync)->queued_head = 0; \
    (sync)->queued_tail = 0

/**
 * @brief Initializes the kernel task manager.
 *
 */
void px_tasks_init();
/**
 * @brief Switches to a provided task.
 *
 * @param task Pointer to the task struct
 */
extern "C" void px_tasks_switch_to(px_task_t *task);
/**
 * @brief Creates a new kernel task with a provided entry point, register storage struct,
 * and task state struct. If the storage parameter is provided, the px_task_t struct
 * provided will be written to. If NULL is passed as the storage parameter, a pointer
 * to a allocated task will be returned.
 *
 * @param entry Task function entry point
 * @param storage Task stack structure (if NULL, a pointer to the task is returned)
 * @param state Task state structure
 * @return px_task_t* Pointer to the created kernel task
 */
px_task_t *px_tasks_new(void (*entry)(void), px_task_t *storage, px_task_state state);
/**
 * @brief Tell the kernel task scheduler to schedule all of the added tasks.
 *
 */
void px_tasks_schedule();
/**
 * @brief Returns the lifetime of the current task (in nanoseconds).
 *
 * @return uint64_t Task lifetime (in nanoseconds)
 */
uint64_t px_tasks_get_self_time();
/**
 * @brief Blocks the current task.
 *
 * @param reason
 */
void px_tasks_block_current(px_task_state reason);
/**
 * @brief Unblocks the current task.
 *
 * @param task
 */
void px_tasks_unblock(px_task_t *task);
/**
 * @brief Sleeps until the provided absolute time (in nanoseconds).
 *
 * @param time Absolute time to sleep until (in nanoseconds since boot)
 */
void px_tasks_nano_sleep_until(uint64_t time);
/**
 * @brief Sleep for a given period of time (in nanoseconds).
 *
 * @param time Nanoseconds to sleep
 */
void px_tasks_nano_sleep(uint64_t time);
/**
 * @brief Exits the current task.
 *
 */
void px_tasks_exit(void);
