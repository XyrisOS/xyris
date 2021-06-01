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

enum task_state
{
    TASK_RUNNING  = 0,
    TASK_READY    = 1,
    TASK_SLEEPING,
    TASK_BLOCKED,
    TASK_STOPPED,
    TASK_PAUSED,
    TASK_STATE_COUNT
};

enum task_alloc { ALLOC_STATIC, ALLOC_DYNAMIC };

typedef struct task task_t;
struct task
{
    uintptr_t stack_top;
    uintptr_t page_dir;
    task_t *next;
    task_state state;
    uint64_t time_used;
    uint64_t wakeup_time;
    const char *name;
    task_alloc alloc;
};

extern task_t *current_task;

#define TASK_ONLY if (current_task != NULL)

typedef struct tasklist
{
    task_t *head;
    task_t *tail;
} tasklist_t;

#define MAX_TASKS_QUEUED 8
typedef struct tasks_sync
{
    task_t* possessor;
    const char *dbg_name;
    tasklist_t waiting;
} tasks_sync_t;

static inline void tasks_sync_init(tasks_sync_t *ts) {
    *ts = {
        .possessor = NULL,
        .dbg_name = NULL,
        .waiting = { },
    };
}

/**
 * @brief Initializes the kernel task manager.
 *
 */
void tasks_init();
/**
 * @brief Switches to a provided task.
 *
 * @param task Pointer to the task struct
 */
extern "C" void tasks_switch_to(task_t *task);
/**
 * @brief Creates a new kernel task with a provided entry point, register storage struct,
 * and task state struct. If the storage parameter is provided, the task_t struct
 * provided will be written to. If NULL is passed as the storage parameter, a pointer
 * to a allocated task will be returned.
 *
 * @param entry Task function entry point
 * @param storage Task stack structure (if NULL, a pointer to the task is returned)
 * @param state Task state structure
 * @return task_t* Pointer to the created kernel task
 */
task_t *tasks_new(void (*entry)(void), task_t *storage, task_state state, const char *name);
/**
 * @brief Tell the kernel task scheduler to schedule all of the added tasks.
 *
 */
void tasks_schedule();
/**
 * @brief Returns the lifetime of the current task (in nanoseconds).
 *
 * @return uint64_t Task lifetime (in nanoseconds)
 */
uint64_t tasks_get_self_time();
/**
 * @brief Blocks the current task.
 *
 * @param reason
 */
void tasks_block_current(task_state reason);
/**
 * @brief Unblocks the current task.
 *
 * @param task
 */
void tasks_unblock(task_t *task);
/**
 * @brief Sleeps until the provided absolute time (in nanoseconds).
 *
 * @param time Absolute time to sleep until (in nanoseconds since boot)
 */
void tasks_nano_sleep_until(uint64_t time);
/**
 * @brief Sleep for a given period of time (in nanoseconds).
 *
 * @param time Nanoseconds to sleep
 */
void tasks_nano_sleep(uint64_t time);
/**
 * @brief Exits the current task.
 *
 */
void tasks_exit(void);

void tasks_sync_block(tasks_sync_t *tsc);

void tasks_sync_unblock(tasks_sync_t *tsc);
