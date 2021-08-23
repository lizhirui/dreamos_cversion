/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-07-07     lizhirui     add vtable and parent field for task
 * 2021-07-08     lizhirui     add brk/init_brk/fd_bitmap/fd_list for task
 */

// @formatter:off
#ifndef __OS_TASK_H__
#define __OS_TASK_H__

    #include <dreamos.h>

    //任务入口函数类型
    typedef os_ssize_t (*task_func_t)(os_size_t arg);

    //任务退出函数类型
    typedef void (*task_exit_func_t)(os_ssize_t exit_code);

    //任务状态枚举
    typedef enum os_task_state
    {
        OS_TASK_STATE_RUNNING,//运行态
        OS_TASK_STATE_READY,//就绪态
        OS_TASK_STATE_BLOCKING,//阻塞态
        OS_TASK_STATE_SLEEPING,//睡眠态
        OS_TASK_STATE_STOPPED,//终止态（任务已经彻底消亡，等待环境清理）
    }os_task_state_t;

    //文件描述符表的前置类型声明
    typedef struct os_file_fd_table os_file_fd_table_t,*os_file_fd_table_p;

    //任务结构体
    typedef struct os_task
    {
        os_size_t sp;//栈顶指针，这个必须在结构体的第一项，以方便上下文切换汇编程序访问
        os_size_t stack_addr;//栈起始地址
        os_size_t stack_size;//栈大小
        struct os_task *parent;//父任务
        char name[OS_VFS_PATH_MAX + 1];//任务名
        char path[OS_VFS_PATH_MAX + 1];//任务路径
        os_size_t pid;//Process ID
        os_size_t tid;//Thread ID
        os_size_t sid;//Session ID
        os_size_t priority;//任务优先级
        os_size_t tick_init;//拥有的时间片
        os_size_t tick_remaining;//剩余时间片
        os_task_state_t task_state;//任务状态
        task_func_t entry;//任务入口
        os_size_t arg;//任务入口参数
        task_exit_func_t exit_func;//任务退出函数（用于任务结束后的环境清理）
        os_ssize_t exit_code;//任务退出码
        os_size_t brk;//堆上界
        os_size_t init_brk;//堆下界
        os_file_fd_table_p fd_table;//文件描述符表
        os_mmu_vtable_p vtable;//页表
        os_list_node_t task_node;//任务列表中的节点
        os_list_node_t schedule_node;//调度列表中的节点
        os_list_node_t child_list;//子任务列表
        os_list_node_t child_node;//子任务列表中的节点
    }os_task_t,*os_task_p;

    os_task_t *os_task_get_current_task();
    os_err_t os_task_init(os_task_p task,os_size_t stack_size,os_size_t priority,os_size_t tick_init,task_func_t entry,os_size_t arg,const char *name);
    void os_task_remove(os_task_p task);
    void os_task_startup(os_task_p task);
    void os_task_yield();
    void os_task_sleep();
    void os_task_wakeup(os_task_t *task);
    os_task_p os_task_get_task_by_pid(os_size_t pid);
    void os_task_schedule();
    os_bool_t os_task_scheduler_is_initialized();
    void os_task_scheduler_init();
    void os_task_scheduler_start();
    void os_task_switch_vtable(os_task_p task);
    void os_task_print_tree(os_task_p task);
    os_task_p os_task_get_root_task();
    os_task_p os_task_get_main_task();

#endif