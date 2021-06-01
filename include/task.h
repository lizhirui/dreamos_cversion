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
        size_t sp;//栈顶指针
        size_t stack_addr;//栈起始地址
        size_t stack_size;//栈大小
        size_t pid;//Process ID
        size_t tid;//Thread ID
        size_t sid;//Session ID
        size_t priority;//任务优先级
        size_t tick_init;//拥有的时间片
        size_t tick_remaining;//剩余时间片
        task_state_t task_state;//任务状态
        task_func_t entry;//任务入口
        size_t arg;//任务入口参数
        task_exit_func_t exit_func;//任务退出函数（用于任务结束后的环境清理）
        ssize_t exit_code;//任务退出码
        list_node task_node;//任务列表中的节点
        list_node schedule_node;//调度列表中的节点
    }task_t;

    task_t *get_current_task();
    void task_init(task_t *task,size_t stack_size,size_t priority,size_t tick_init,task_func_t entry,size_t arg);
    void task_schedule();
    void task_scheduler_init();
    void task_scheduler_start();

#endif