/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 */

// @formatter:off
#include <dreamos.h>

/*!
 * 互斥锁初始化
 * @param mutex 互斥锁结构体指针
 */
void os_mutex_init(os_mutex_p mutex)
{
    mutex -> locked = OS_FALSE;
    mutex -> owner = OS_NULL;
    mutex -> refcnt = 0;
    os_waitqueue_init(&mutex -> waitqueue);
}

/*!
 * 互斥锁锁定，支持递归调用
 * @param mutex 互斥锁结构体指针
 */
void os_mutex_lock(os_mutex_p mutex)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    OS_ENTER_CRITICAL_AREA();

    //若已经处于锁定状态，若当前任务就是该锁的拥有者，则锁的引用数自增，否则将当前任务加入该锁的等待列表，否则执行加锁操作
    if(mutex -> locked)
    {
        if(mutex -> owner == os_task_get_current_task())
        {
            mutex -> refcnt++;
        }
        else
        {
            os_waitqueue_wait(&mutex -> waitqueue);
        }
    }
    else
    {
        mutex -> refcnt = 1;
        mutex -> owner = os_task_get_current_task();
        mutex -> locked = OS_TRUE;
    }
    
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 互斥锁解锁，支持递归调用
 * @param mutex 互斥锁结构体指针
 */
void os_mutex_unlock(os_mutex_p mutex)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    OS_ENTER_CRITICAL_AREA();
    OS_ASSERT(mutex -> owner == os_task_get_current_task());
    OS_ASSERT(mutex -> locked);
    OS_ASSERT(mutex -> refcnt > 0);
    //引用数递减
    mutex -> refcnt--;

    //若引用数变为0，则唤醒等待列表中的一个任务
    if(mutex -> refcnt == 0)
    {
        os_task_p next_task = os_waitqueue_get_head(&mutex -> waitqueue);

        /*
         * 若等待列表中存在一个任务，则让该任务直接成为锁的拥有者，并设置锁的引用数为1，然后唤醒该任务，这里切不可直接唤醒，否则会导致在未完成
         * 加锁操作的情况下，该任务获得锁；若不存在任何任务，则释放该锁
         */
        if(next_task != OS_NULL)
        {
            mutex -> refcnt = 1;
            mutex -> owner = next_task;
            os_waitqueue_wakeup(&mutex -> waitqueue);
        }
        else
        {
            mutex -> owner = OS_NULL;
            mutex -> locked = OS_FALSE;
        }
    }
    
    OS_LEAVE_CRITICAL_AREA();
}