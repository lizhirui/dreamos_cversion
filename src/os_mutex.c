/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 */

#include <dreamos.h>

void os_mutex_init(os_mutex_p mutex)
{
    mutex -> locked = OS_FALSE;
    mutex -> owner = OS_NULL;
    mutex -> refcnt = 0;
    os_waitqueue_init(&mutex -> waitqueue);
}

void os_mutex_lock(os_mutex_p mutex)
{
    OS_ANNOTATION_NEED_THREAD_CONTEXT();
    OS_ENTER_CRITICAL_AREA();

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

void os_mutex_unlock(os_mutex_p mutex)
{
    OS_ANNOTATION_NEED_THREAD_CONTEXT();
    OS_ENTER_CRITICAL_AREA();
    OS_ASSERT(mutex -> owner == os_task_get_current_task());
    OS_ASSERT(mutex -> locked);
    OS_ASSERT(mutex -> refcnt > 0);
    mutex -> refcnt--;

    if(mutex -> refcnt == 0)
    {
        os_task_p next_task = os_waitqueue_get_head(&mutex -> waitqueue);

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