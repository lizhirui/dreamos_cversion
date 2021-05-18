/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __TASK_H__
#define __TASK_H__

    #include <dreamos.h>

    typedef void (*task_func_t)(size_t arg);
    typedef void (*task_exit_func_t)(ssize_t exit_code);

    typedef enum task_state
    {
        TASK_STATE_RUNNING,
        TASK_STATE_READY,
        TASK_STATE_BLOCKING,
        TASK_STATE_SLEEPING,
        TASK_STATE_STOPPED,
    }task_state_t;

    typedef struct task
    {
        size_t sp;
        size_t stack_addr;
        size_t stack_size;
        size_t pid;
        size_t tid;
        size_t sid;
        size_t priority;
        size_t tick_init;
        size_t tick_remaining;
        task_state_t task_state;
        task_func_t entry;
        size_t arg;
        task_exit_func_t exit_func;
        ssize_t exit_code;
        list_node task_node;
        list_node schedule_node;
    }task_t;

    task_t *get_current_task();
    void task_init(task_t *task,size_t stack_size,size_t priority,size_t tick_init,task_func_t entry,size_t arg);
    void task_schedule();
    void task_scheduler_init();
    void task_scheduler_start();

#endif