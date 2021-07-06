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

void os_waitqueue_add(os_waitqueue_p waitqueue,os_waitqueue_node_p node)
{
    OS_ENTER_CRITICAL_AREA();
    os_list_insert_tail(waitqueue -> waiting_list,&node -> node);
    OS_LEAVE_CRITICAL_AREA();
}

void os_waitqueue_remove(os_waitqueue_node_p node)
{
    OS_ENTER_CRITICAL_AREA();
    os_list_node_remove(&node -> node);
    OS_LEAVE_CRITICAL_AREA();
}

void os_waitqueue_wait(os_waitqueue_p waitqueue)
{
    OS_ANNOTATION_NEED_THREAD_CONTEXT();
    os_waitqueue_node_t node;
    node.task = os_task_get_current_task();
    os_waitqueue_add(waitqueue,&node);
    os_task_sleep();
}

void os_waitqueue_wakeup(os_waitqueue_p waitqueue)
{
    OS_ENTER_CRITICAL_AREA();

    if(!os_list_empty(waitqueue -> waiting_list))
    {
        os_waitqueue_node_p node = os_list_entry(os_list_get_head(waitqueue -> waiting_list),os_waitqueue_node_t,node);
        os_waitqueue_remove(node);
        os_task_wakeup(node -> task);
    }

    OS_LEAVE_CRITICAL_AREA();
}

os_bool_t os_waitqueue_empty(os_waitqueue_p waitqueue)
{
    return os_list_empty(waitqueue -> waiting_list);
}

os_task_p os_waitqueue_get_head(os_waitqueue_p waitqueue)
{
    if(!os_list_empty(waitqueue -> waiting_list))
    {
        return os_list_entry(os_list_get_head(waitqueue -> waiting_list),os_waitqueue_node_t,node) -> task;
    }
    else
    {
        return OS_NULL;
    }
}

void os_waitqueue_init(os_waitqueue_p waitqueue)
{
    os_list_init(waitqueue -> waiting_list);
}