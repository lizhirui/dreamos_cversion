/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-07-06     lizhirui     add task tree support
 * 2021-07-07     lizhirui     add pid support
 * 2021-07-08     lizhirui     add fd list/bitmap and brk/init_brk fields support for task
 * 2021-07-09     lizhirui     add fd_table support
 */

// @formatter:off
#include <dreamos.h>

//任务优先级开区间上界（即实际优先级的的数量）
#define TASK_PRIORITY_UPLIMIT (TASK_PRIORITY_MAX + 1)

static os_list_node_t task_list;//任务列表
static os_list_node_t priority_ready_list[TASK_PRIORITY_UPLIMIT];//按任务优先级存储的任务就绪列表

os_task_t *current_task = OS_NULL;//当前任务

//以下三个变量用于在中断中请求任务切换时推迟请求
os_bool_t need_lazy_task_switch = OS_FALSE;//是否有挂起的任务切换请求
os_task_t *lazy_old_task = OS_NULL;//切换来源任务
os_task_t *lazy_next_task = OS_NULL;//切换目标任务

//调度器是否初始化完成
static os_bool_t os_task_scheduler_initialized = OS_FALSE;

static os_task_t task_idle;//idle任务结构体
static os_task_t task_main;//main任务结构体

void arch_task_switch(os_task_t *old_task,os_task_t *new_task);
void arch_task_stack_frame_init(os_task_t *task);

/*!
 * 获取当前的任务
 * @return 若调度器已启动，则该函数返回当前的任务，否则返回OS_NULL
 */
os_task_t *os_task_get_current_task()
{
    return current_task;
}

/*!
 * 获取下一个任务，该函数按照优先级从高到低遍历priority_ready_list，返回优先级最高的就绪任务，当同一优先级存在多个就绪任务时，返回靠前的任务
 * @return 优先级最高的就绪任务
 */
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

/*!
 * 返回任务错误码
 * @param exit_code 错误码
 */
void task_exit(os_ssize_t exit_code)
{
    current_task -> exit_code = exit_code;
    while(1);
}

/*!
 * 执行任务调度，该函数将会选出下一个应当执行的任务，若下一任务与当前任务不同，将会触发任务切换行为
 */
void os_task_schedule()
{
    OS_ENTER_CRITICAL_AREA();
    os_task_t *next_task = get_next_task();
    OS_ASSERT(next_task != OS_NULL);

    //优先级检测，若当前任务仍处于运行态且新任务优先级更低，则继续执行当前任务
    if(current_task -> task_state == OS_TASK_STATE_RUNNING)
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

        if(current_task -> task_state == OS_TASK_STATE_RUNNING)
        {
            os_list_insert_tail(priority_ready_list[current_task -> priority],&current_task -> schedule_node);
            current_task -> task_state = OS_TASK_STATE_READY;
        }

        //时间片重置
        if(current_task -> tick_remaining == 0)
        {
            current_task -> tick_remaining = current_task -> tick_init;
        }

        os_task_t *old_task = current_task;
        current_task = next_task;
        current_task -> task_state = OS_TASK_STATE_RUNNING;

        //若当前在中断上下文中，则推迟任务切换，否则立即进行任务切换
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

/*!
 * 获取任务内核栈顶，该函数主要用于汇编程序，用于从中断栈切换到线程内核栈时使用
 * @param task 要获取内核栈顶的任务
 * @return 内核栈顶地址
 */
os_size_t os_task_get_kernel_stack_top(os_task_t *task)
{
    return task -> stack_addr + task -> stack_size;
}

static os_bitmap_t os_task_pid_bitmap;//任务pid位图，其中每一位为1表示空闲，为0表示占用
static os_hashmap_t os_task_pid_to_task_hashmap;//用于将任务pid映射到任务结构体指针

/*!
 * 获取一个新的pid，该函数在获取的同时，会将该pid在位图中标记为占用状态
 * @param pid 新的pid
 * @return 成功返回OS_ERR_OK，失败返回-OS_ERR_EPERM（通常由于无空闲的pid导致）
 */
static os_err_t os_task_get_new_pid(os_size_t *pid)
{
    OS_ASSERT(pid != OS_NULL);
    os_size_t ret = os_bitmap_find_some_ones(&os_task_pid_bitmap,0,1);

    if(ret == OS_NUMBER_MAX(os_size_t))
    {
        return OS_ERR_EPERM;
    }

    *pid = ret;
    os_bitmap_set_bit(&os_task_pid_bitmap,ret,0);
    return OS_ERR_OK;
}

/*!
 * 释放一个pid，通常用于任务初始化失败或任务销毁时
 * @param 要释放的pid
 */
static void os_task_release_pid(os_size_t pid)
{
    os_bitmap_set_bit(&os_task_pid_bitmap,pid,1);
}

/*!
 * 初始化一个任务结构体
 * @param task 任务结构体指针
 * @param stack_size 任务内核栈大小
 * @param priority 任务优先级，该值必须小于TASK_PRIORITY_MAX，否则会导致EPERM错误
 * @param tick_init 时间片数量
 * @param entry 任务入口
 * @param arg 传递给任务入口的参数
 * @param name 任务名称
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_task_init(os_task_p task,os_size_t stack_size,os_size_t priority,os_size_t tick_init,task_func_t entry,os_size_t arg,const char *name)
{
    OS_ANNOTATION_NEED_NON_INTERRUPT_CONTEXT();
    OS_ASSERT(task != OS_NULL);

    if(priority > TASK_PRIORITY_MAX)
    {
        return -OS_ERR_EPERM;
    }

    //创建内核栈
    task -> stack_addr = (os_size_t)os_memory_alloc(stack_size);
    
    if(!task -> stack_addr)
    {
        return -OS_ERR_ENOMEM;
    }

    //初始化任务名称和任务路径（内核任务路径为空，用户任务才有路径信息）
    os_memset(task -> name,0,sizeof(task -> name));
    os_memset(task -> path,0,sizeof(task -> path));

    if(name != OS_NULL)
    {
        os_strcpy(task -> name,name);
    }

    //申请新的pid
    os_err_t err = os_task_get_new_pid(&task -> pid);

    if(err != OS_ERR_OK)
    {
        os_memory_free((void *)task -> stack_addr);
        return err;
    }

    //将pid与任务结构体进行关联
    if((err = os_hashmap_set(&os_task_pid_to_task_hashmap,task -> pid,task)) != OS_ERR_OK)
    {
        os_task_release_pid(task -> pid);
        os_memory_free((void *)task -> stack_addr);
        return err;
    }

    //创建文件描述符表
    if((task -> fd_table = os_file_fd_table_create()) == OS_NULL)
    {
        os_hashmap_remove_item(&os_task_pid_to_task_hashmap,task -> pid);
        os_task_release_pid(task -> pid);
        os_memory_free((void *)task -> stack_addr);
        return err;
    }

    task -> stack_size = stack_size;
    task -> sp = task -> stack_addr + task -> stack_size;
    task -> priority = priority;
    task -> tick_init = tick_init;
    task -> tick_remaining = tick_init;
    task -> entry = entry;
    task -> arg = arg;
    task -> task_state = OS_TASK_STATE_READY;//让任务初始处于就绪态
    task -> exit_func = task_exit;//设置任务退出时的错误码处理和环境清理函数
    //将任务的页表设置为内核页表，并将内核页表引用数+1
    task -> vtable = os_mmu_get_kernel_pagetable();
    task -> vtable -> refcnt++;
    //将当前任务作为新任务的父任务构成任务树
    task -> parent = os_task_get_current_task();
    //初始化初始brk边界和当前brk边界，仅用于用户任务，因此此处设置为0
    task -> init_brk = 0;
    task -> brk = 0;
    //初始化新任务的子任务列表
    os_list_init(task -> child_list);
    //初始化新任务的任务列表节点和调度列表节点
    os_list_node_init(&task -> task_node);
    os_list_node_init(&task -> schedule_node);

    //将新任务加入当前的任务的子任务列表
    if(task -> parent != OS_NULL)
    {
        os_list_insert_tail(task -> parent -> child_list,&task -> child_node);
    }

    //初始化新任务的入口上下文
    arch_task_stack_frame_init(task);
    return OS_ERR_OK;
}

/*!
 * 销毁任务，该函数用于将任务结构体从内核中剥离，一般仅用于任务自身的环境清理，请不要在任务运行时调用该函数，否则可能导致系统崩溃
 * @param task 要销毁的任务
 */
void os_task_remove(os_task_p task)
{
    OS_ENTER_CRITICAL_AREA();
    OS_ASSERT(task != &task_idle);
    OS_ASSERT(task != &task_main);
    OS_ASSERT(task -> parent != OS_NULL);
    os_memory_free((void *)task -> stack_addr);
    os_list_node_remove(&task -> task_node);
    os_list_node_remove(&task -> child_node);

    if(!os_list_node_empty(&task -> schedule_node))
    {
        os_list_node_remove(&task -> schedule_node);
    }

    os_list_entry_foreach_safe(task -> child_list,os_task_t,child_node,entry,
    {
        os_list_node_remove(&entry -> child_node);
        os_list_insert_tail(task -> parent -> child_list,&entry -> child_node);
        entry -> parent = task -> parent;
    });

    task -> vtable -> refcnt--;

    if(task -> vtable -> refcnt == 0)
    {
        os_mmu_vtable_remove(task -> vtable,OS_TRUE);
        os_memory_free(task -> vtable);
    }

    os_file_fd_table_remove(task -> fd_table);
    //wait fd_list remove code
    os_memory_free(task);
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 启动任务，该任务必须首先被初始化，否则行为不可预测
 * @param task 要启动的任务
 */
void os_task_startup(os_task_p task)
{
    OS_ANNOTATION_NEED_NON_INTERRUPT_CONTEXT();
    OS_ASSERT(task != OS_NULL);
    OS_ENTER_CRITICAL_AREA();
    os_list_insert_tail(task_list,&task -> task_node);
    os_list_insert_tail(priority_ready_list[task -> priority],&task -> schedule_node);
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 设置当前任务状态
 * @param state 新的任务状态
 */
static void set_current_task_state(os_task_state_t state)
{
    OS_ENTER_CRITICAL_AREA();
    os_task_t *task = os_task_get_current_task();
    OS_ASSERT(task != OS_NULL);
    task -> task_state = state;
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 立即放弃所有的剩余时间片
 */
void os_task_yield()
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    OS_ENTER_CRITICAL_AREA();
    os_task_schedule();
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 让当前任务进入睡眠态
 */
void os_task_sleep()
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    OS_ENTER_CRITICAL_AREA();
    set_current_task_state(OS_TASK_STATE_SLEEPING);
    os_task_schedule();
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 唤醒任务
 * @param task 要唤醒的任务
 */
void os_task_wakeup(os_task_t *task)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    OS_ENTER_CRITICAL_AREA();

    //只有睡眠状态的任务才能被唤醒
    if(task -> task_state == OS_TASK_STATE_SLEEPING)
    {
        //将任务设置为就绪状态
        task -> task_state = OS_TASK_STATE_READY;
        os_list_insert_tail(priority_ready_list[task -> priority],&task -> schedule_node);
        //立即执行调度
        os_task_schedule();
    }
    
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 通过pid获取任务结构体指针
 * @param pid
 * @return 若pid无效，则返回OS_NULL
 */
os_task_p os_task_get_task_by_pid(os_size_t pid)
{
    os_task_p ret;
    os_hashmap_get(&os_task_pid_to_task_hashmap,pid,(void **)&ret);
    return ret;
}

/*!
 * 启动的第二个线程，用于在线程上下文下完成剩余的初始化工作，处于bsp目录中
 * @param arg 要传递的参数
 * @return 线程退出码
 */
os_ssize_t os_task_main_entry(os_size_t arg);

/*!
 * 启动的第一个线程，idle线程入口
 * @param arg 要传递的参数
 * @return 线程退出码
 */
OS_NORETURN os_ssize_t os_task_idle_entry(os_size_t arg)
{
    //初始化启动main线程
    OS_ASSERT(os_task_init(&task_main,MAIN_TASK_STACK_SIZE,MAIN_TASK_PRIORITY,MAIN_TASK_TICK_INIT,os_task_main_entry,0,"task_main") == OS_ERR_OK);
    os_task_startup(&task_main);

    //执行空闲操作
    while(1)
    {
        os_task_yield();
    }
}

/*!
 * 判断调度器是否已经完成初始化
 * @return 调度器初始化完成返回OS_TRUE，否则返回OS_FALSE
 */
os_bool_t os_task_scheduler_is_initialized()
{
    return os_task_scheduler_initialized;
}

/*!
 * 任务调度器初始化
 */
void os_task_scheduler_init()
{
    os_size_t i;

    os_list_init(task_list);

    for(i = 0;i < TASK_PRIORITY_UPLIMIT;i++)
    {
        os_list_init(priority_ready_list[i]);
    }

    OS_ASSERT(os_bitmap_create(&os_task_pid_bitmap,OS_TASK_MAX_NUM,OS_NULL,1) == OS_ERR_OK);
    OS_ASSERT(os_hashmap_create(&os_task_pid_to_task_hashmap,MIN(1000,OS_TASK_MAX_NUM),OS_NULL) == OS_ERR_OK);
    OS_ASSERT(os_task_init(&task_idle,IDLE_TASK_STACK_SIZE,TASK_PRIORITY_MAX,IDLE_TASK_TICK_INIT,os_task_idle_entry,0,"task_idle") == OS_ERR_OK);
    os_task_startup(&task_idle);
    os_task_scheduler_initialized = OS_TRUE;
}

/*!
 * 任务调度器启动，从这时候开始，就进入了第一个任务上下文，该函数不会返回
 */
void os_task_scheduler_start()
{
    current_task = &task_idle;
    current_task -> task_state = OS_TASK_STATE_RUNNING;
    os_list_node_remove(&current_task -> schedule_node);
    arch_task_switch(OS_NULL,current_task);
}

/*!
 * 切换到指定任务的页表
 * @param task 任务结构体指针
 */
void os_task_switch_vtable(os_task_p task)
{
    os_mmu_switch(task -> vtable);
}

/*!
 * 用于打印任务树的递归函数
 * @param task 任务结构体指针
 * @param level 当前层次
 */
static void __os_task_print_tree(os_task_p task,os_size_t level)
{
    os_size_t i;

    OS_ENTER_CRITICAL_AREA();
    
    os_list_entry_foreach(task -> child_list,os_task_t,child_node,entry,
    {
        for(i = 0;i < level;i++)
        {
            os_printf("|  ");
        }

        os_printf("|--%d - %s - 0x%p - %s\n",entry -> pid,entry -> name,entry,(os_task_get_task_by_pid(task -> pid) == task) ? "pid map normal" : "pid map error");
        __os_task_print_tree(entry,level + 1);
    });

    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 打印任务树
 * @param task 要打印任务树的根任务
 */
void os_task_print_tree(os_task_p task)
{
    os_printf("Task Tree:\n");
    os_printf("---------------------------\n\n");
    os_printf("%d - %s - 0x%p - %s\n",task -> pid,task -> name,task,(os_task_get_task_by_pid(task -> pid) == task) ? "pid map normal" : "pid map error");
    __os_task_print_tree(task,0);
    os_printf("\n---------------------------\n");
    os_printf("\n");
}

/*!
 * 获取根任务
 * @return 根任务结构体指针
 */
os_task_p os_task_get_root_task()
{
    return &task_idle;
}

/*!
 * 获取main任务
 * @return main任务结构体指针
 */
os_task_p os_task_get_main_task()
{
    return &task_main;
}