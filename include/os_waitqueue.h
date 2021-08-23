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

    //等待队列结构体
    typedef struct os_waitqueue
    {
        os_list_node_t waiting_list;//等待队列节点列表
    }os_waitqueue_t,*os_waitqueue_p;

    //等待队列节点结构体
    typedef struct os_waitqueue_node
    {
        os_task_p task;//关联任务
        os_list_node_t node;//关联列表节点
    }os_waitqueue_node_t,*os_waitqueue_node_p;

    void os_waitqueue_add(os_waitqueue_p waitqueue,os_waitqueue_node_p node);
    void os_waitqueue_remove(os_waitqueue_node_p node);
    void os_waitqueue_wait(os_waitqueue_p waitqueue);
    void os_waitqueue_wakeup(os_waitqueue_p waitqueue);
    os_bool_t os_waitqueue_empty(os_waitqueue_p waitqueue);
    os_task_p os_waitqueue_get_head(os_waitqueue_p waitqueue);
    void os_waitqueue_init(os_waitqueue_p waitqueue);

#endif