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
#ifndef __OS_WAITQUEUE_H__
#define __OS_WAITQUEUE_H__

    #include <dreamos.h>

    typedef struct os_waitqueue
    {
        os_list_node_t waiting_list;
    }os_waitqueue_t,*os_waitqueue_p;

    typedef struct os_waitqueue_node
    {
        os_task_p task;
        os_list_node_t node;
    }os_waitqueue_node_t,*os_waitqueue_node_p;

    void os_waitqueue_add(os_waitqueue_p waitqueue,os_waitqueue_node_p node);
    void os_waitqueue_remove(os_waitqueue_node_p node);
    void os_waitqueue_wait(os_waitqueue_p waitqueue);
    void os_waitqueue_wakeup(os_waitqueue_p waitqueue);
    os_bool_t os_waitqueue_empty(os_waitqueue_p waitqueue);
    os_task_p os_waitqueue_get_head(os_waitqueue_p waitqueue);
    void os_waitqueue_init(os_waitqueue_p waitqueue);

#endif