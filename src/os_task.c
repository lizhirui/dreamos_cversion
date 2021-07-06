/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-07-06     lizhirui     add task tree support
 */

#include <dreamos.h>

#define TASK_PRIORITY_UPLIMIT (TASK_PRIORITY_MAX + 1)

static os_list_node_t task_list;
static os_list_node_t priority_ready_list[TASK_PRIORITY_UPLIMIT];

os_task_t *current_task = OS_NULL;
os_bool_t need_lazy_task_switch = OS_FALSE;
os_task_t *lazy_old_task = OS_NULL;
os_task_t *lazy_next_task = OS_NULL;

static os_bool_t os_task_scheduler_initialized = OS_FALSE;

void arch_task_switch(os_task_t *old_task,os_task_t *new_task);
void arch_task_stack_frame_init(os_task_t *task);

//获取当前的任务
os_task_t *os_task_get_current_task()
{
    return current_task;
}

//获取下一个任务
static os_task_t *get_next_task()
{
    os_size_t i;

    //按优先级查表
    for(i = 0;i < TASK_PRIORITY_UPLIMIT;i++)
    {
        if(!os_list_empty(priority_ready_list[i]))
        {
            return os_list_entry(os_list_get_head(priority_ready_list[i]),os_task_t,schedule_node);
        }
    }

    return OS_NULL;
}

//设置任务退出码
void task_exit(os_ssize_t exit_code)
{
    current_task -> exit_code = exit_code;
    while(1);
}

//执行任务调度
void os_task_schedule()
{
    OS_ENTER_CRITICAL_AREA();
    os_task_t *next_task = get_next_task();
    OS_ASSERT(next_task != OS_NULL);

    //优先级检测，若当前任务仍处于运行态且新任务优先级更低，则继续执行当前任务
    if(current_task -> os_task_state == OS_TASK_STATE_RUNNING)
    {
        if(next_task -> priority > current_task -> priority)
        {
            next_task = current_task;
        }
    }

    if((current_task -> sp < current_task -> stack_addr) || (current_task -> sp > (current_task -> stack_addr + current_task -> stack_size)))
    {
        os_printf("stack overflow!\n");
        while(1);
    }
    
    //检查是否需要进行任务切换
    if(next_task != current_task)
    {
        os_list_node_remove(&next_task -> schedule_node);

        if(current_task -> os_task_state == OS_TASK_STATE_RUNNING)
        {
            os_list_insert_tail(priority_ready_list[current_task -> priority],&current_task -> schedule_node);
            current_task -> os_task_state = OS_TASK_STATE_READY;
        }

        //时间片重置
        if(current_task -> tick_remaining == 0)
        {
            current_task -> tick_remaining = current_task -> tick_init;
        }

        os_task_t *old_task = current_task;
        current_task = next_task;
        current_task -> os_task_state = OS_TASK_STATE_RUNNING;

        if(os_is_in_interrupt())
        {
            need_lazy_task_switch = OS_TRUE;
            lazy_old_task = old_task;
            lazy_next_task = next_task;
        }
        else
        {
            arch_task_switch(old_task,next_task);
        }
    }

    OS_LEAVE_CRITICAL_AREA();
}

//获取任务内核栈顶
os_size_t os_task_get_kernel_stack_top(os_task_t *task)
{
    return task -> stack_addr + task -> stack_size;
}

//初始化一个任务结构体
os_err_t os_task_init(os_task_p task,os_size_t stack_size,os_size_t priority,os_size_t tick_init,task_func_t entry,os_size_t arg)
{
    OS_ANNOTATION_NEED_NON_INTERRUPT_CONTEXT();
    OS_ASSERT(task != OS_NULL);
    task -> stack_addr = (os_size_t)os_memory_alloc(stack_size);
    
    if(!task -> stack_addr)
    {
        return -OS_ERR_ENOMEM;
    }

    task -> stack_size = stack_size;
    task -> sp = task -> stack_addr + task -> stack_size;
    task -> priority = priority;
    task -> tick_init = tick_init;
    task -> tick_remaining = tick_init;
    task -> entry = entry;
    task -> arg = arg;
    task -> os_task_state = OS_TASK_STATE_READY;
    task -> exit_func = task_exit;
    task -> vtable = os_mmu_get_kernel_pagetable();
    task -> parent = os_task_get_current_task();
    arch_task_stack_frame_init(task);
    return OS_ERR_OK;
}

void os_task_startup(os_task_p task)
{
    OS_ANNOTATION_NEED_NON_INTERRUPT_CONTEXT();
    OS_ASSERT(task != OS_NULL);
    OS_ENTER_CRITICAL_AREA();
    os_list_insert_tail(task_list,&task -> task_node);
    os_list_insert_tail(priority_ready_list[task -> priority],&task -> schedule_node);
    OS_LEAVE_CRITICAL_AREA();
}

//设置当前任务状态
static void set_current_task_state(task_state_t state)
{
    OS_ENTER_CRITICAL_AREA();
    os_task_t *task = os_task_get_current_task();
    OS_ASSERT(task != OS_NULL);
    task -> os_task_state = state;
    OS_LEAVE_CRITICAL_AREA();
}

//立即放弃控制权
void os_task_yield()
{
    OS_ANNOTATION_NEED_THREAD_CONTEXT();
    OS_ENTER_CRITICAL_AREA();
    os_task_schedule();
    OS_LEAVE_CRITICAL_AREA();
}

//任务睡眠
void os_task_sleep()
{
    OS_ANNOTATION_NEED_THREAD_CONTEXT();
    OS_ENTER_CRITICAL_AREA();
    set_current_task_state(OS_TASK_STATE_SLEEPING);
    os_task_schedule();
    OS_LEAVE_CRITICAL_AREA();
}

//任务唤醒
void os_task_wakeup(os_task_t *task)
{
    OS_ANNOTATION_NEED_THREAD_CONTEXT();
    OS_ENTER_CRITICAL_AREA();
    
    if(task -> os_task_state == OS_TASK_STATE_SLEEPING)
    {
        task -> os_task_state = OS_TASK_STATE_READY;
        os_list_insert_tail(priority_ready_list[task -> priority],&task -> schedule_node);
        os_task_schedule();
    }
    
    OS_LEAVE_CRITICAL_AREA();
}

static os_task_t task_idle;
static os_task_t task_main;

os_ssize_t os_task_main_entry(os_size_t arg);

//idle任务入口函数
os_ssize_t os_task_idle_entry(os_size_t arg)
{
    OS_ASSERT(os_task_init(&task_main,MAIN_TASK_STACK_SIZE,MAIN_TASK_PRIORITY,MAIN_TASK_TICK_INIT,os_task_main_entry,0) == OS_ERR_OK);
    os_task_startup(&task_main);

    while(1)
    {
        os_task_yield();
    }
}

os_bool_t os_task_scheduler_is_initialized()
{
    return os_task_scheduler_initialized;
}

//任务调度器初始化
void os_task_scheduler_init()
{
    os_size_t i;

    os_list_init(task_list);

    for(i = 0;i < TASK_PRIORITY_UPLIMIT;i++)
    {
        os_list_init(priority_ready_list[i]);
    }

    OS_ASSERT(os_task_init(&task_idle,IDLE_TASK_STACK_SIZE,TASK_PRIORITY_MAX,IDLE_TASK_TICK_INIT,os_task_idle_entry,0) == OS_ERR_OK);
    os_task_startup(&task_idle);
    os_task_scheduler_initialized = OS_TRUE;
}

//任务调度器启动
void os_task_scheduler_start()
{
    current_task = &task_idle;
    current_task -> os_task_state = OS_TASK_STATE_RUNNING;
    os_list_node_remove(&current_task -> schedule_node);
    arch_task_switch(OS_NULL,current_task);
}

void os_task_switch_vtable(os_task_p task)
{
    os_mmu_switch(task -> vtable);
}