/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#include <dreamos.h>

#define TASK_PRIORITY_UPLIMIT (TASK_PRIORITY_MAX + 1)

static list_node task_list;
static list_node priority_ready_list[TASK_PRIORITY_UPLIMIT];

task_t *current_task = NULL;
bool_t need_lazy_task_switch = FALSE;
task_t *lazy_old_task = NULL;
task_t *lazy_next_task = NULL;

void arch_task_switch(task_t *old_task,task_t *new_task);
void arch_task_stack_frame_init(task_t *task);

//获取当前的任务
task_t *get_current_task()
{
    return current_task;
}

//获取下一个任务
static task_t *get_next_task()
{
    size_t i;

    for(i = 0;i < TASK_PRIORITY_UPLIMIT;i++)
    {
        if(!list_empty(priority_ready_list[i]))
        {
            return list_entry(list_get_head(priority_ready_list[i]),task_t,schedule_node);
        }
    }

    return NULL;
}

//设置任务退出码
void task_exit(ssize_t exit_code)
{
    current_task -> exit_code = exit_code;
    while(1);
}

//执行任务调度
void task_schedule()
{
    ENTER_CRITICAL_AREA();
    task_t *next_task = get_next_task();
    OS_ASSERT(next_task != NULL);

    if(current_task -> task_state == TASK_STATE_RUNNING)
    {
        if(next_task -> priority > current_task -> priority)
        {
            next_task = current_task;
        }
    }
    
    if(next_task != current_task)
    {
        list_node_remove(&next_task -> schedule_node);

        if(current_task -> task_state == TASK_STATE_RUNNING)
        {
            list_insert_tail(priority_ready_list[current_task -> priority],&current_task -> schedule_node);
            current_task -> task_state = TASK_STATE_READY;
        }

        if(current_task -> tick_remaining == 0)
        {
            current_task -> tick_remaining = current_task -> tick_init;
        }

        task_t *old_task = current_task;
        current_task = next_task;
        current_task -> task_state = TASK_STATE_RUNNING;

        if(is_in_interrupt())
        {
            need_lazy_task_switch = TRUE;
            lazy_old_task = old_task;
            lazy_next_task = next_task;
        }
        else
        {
            arch_task_switch(old_task,next_task);
        }
    }

    LEAVE_CRITICAL_AREA();
}

//获取任务内核栈顶
size_t task_get_kernel_stack_top(task_t *task)
{
    return task -> stack_addr + task -> stack_size;
}

//初始化一个任务结构体
void task_init(task_t *task,size_t stack_size,size_t priority,size_t tick_init,task_func_t entry,size_t arg)
{
    task -> stack_addr = (size_t)phypage_alloc(stack_size);
    OS_ASSERT(task -> stack_addr != ((size_t)NULL));
    task -> stack_size = stack_size;
    task -> sp = task -> stack_addr + task -> stack_size;
    task -> priority = priority;
    task -> tick_init = tick_init;
    task -> tick_remaining = tick_init;
    task -> entry = entry;
    task -> arg = arg;
    task -> task_state = TASK_STATE_READY;
    task -> exit_func = task_exit;
    arch_task_stack_frame_init(task);
    list_insert_tail(task_list,&task -> task_node);
    list_insert_tail(priority_ready_list[task -> priority],&task -> schedule_node);
}

static task_t task_idle;
static task_t task_main;

void task_main_entry(size_t arg);

static task_t task_test;

//idle任务入口函数
void task_idle_entry(size_t arg)
{
    while(1)
    {
        os_printf("idle\n");
    }
}

//test任务入口函数
void task_test_entry()
{
    while(1)
    {
        os_printf("task_test_entry\n");
    }
}

//任务调度器初始化
void task_scheduler_init()
{
    size_t i;

    list_init(task_list);

    for(i = 0;i < TASK_PRIORITY_UPLIMIT;i++)
    {
        list_init(priority_ready_list[i]);
    }

    task_init(&task_idle,IDLE_TASK_STACK_SIZE,TASK_PRIORITY_MAX,IDLE_TASK_TICK_INIT,task_idle_entry,0);
    task_init(&task_main,MAIN_TASK_STACK_SIZE,MAIN_TASK_PRIORITY,MAIN_TASK_TICK_INIT,task_main_entry,0);
    task_init(&task_test,MAIN_TASK_STACK_SIZE,MAIN_TASK_PRIORITY,MAIN_TASK_TICK_INIT,task_test_entry,0);
}

//任务调度器启动
void task_scheduler_start()
{
    current_task = &task_main;
    current_task -> task_state = TASK_STATE_RUNNING;
    list_node_remove(&task_main.schedule_node);
    arch_task_switch(NULL,&task_main);
}