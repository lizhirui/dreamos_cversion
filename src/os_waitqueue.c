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
 * 向等待队列中加入节点，通常由os_waitqueue_wait调用，而不是由外部直接调用
 * @param waitqueue 等待队列结构体指针
 * @param node 节点
 */
void os_waitqueue_add(os_waitqueue_p waitqueue,os_waitqueue_node_p node)
{
    OS_ENTER_CRITICAL_AREA();
    os_list_insert_tail(waitqueue -> waiting_list,&node -> node);
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 从等待队列中移除节点，通常由os_waitqueue_wait调用，而不是由外部直接调用
 * @param node 节点
 */
void os_waitqueue_remove(os_waitqueue_node_p node)
{
    OS_ENTER_CRITICAL_AREA();
    os_list_node_remove(&node -> node);
    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 让当前任务在指定的等待队列中等待，执行该函数后，当前任务会进入睡眠态
 * @param waitqueue 等待队列结构体指针
 */
void os_waitqueue_wait(os_waitqueue_p waitqueue)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    os_waitqueue_node_t node;
    node.task = os_task_get_current_task();
    os_waitqueue_add(waitqueue,&node);
    os_task_sleep();
}

/*!
 * 唤醒等待队列中的一个任务，若队列为空，则不动作
 * @param waitqueue 等待队列结构体指针
 */
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

/*!
 * 检测等待队列是否为空
 * @param waitqueue 等待队列结构体指针
 * @return 为空返回OS_TRUE，否则返回OS_FALSE
 */
os_bool_t os_waitqueue_empty(os_waitqueue_p waitqueue)
{
    return os_list_empty(waitqueue -> waiting_list);
}

/*!
 * 获取等待队列的头部任务
 * @param waitqueue 等待队列结构体指针
 * @return 若等待队列不为空，则返回任务结构体指针，否则返回OS_NULL
 */
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

/*!
 * 初始化等待队列
 * @param waitqueue 等待队列结构体指针
 */
void os_waitqueue_init(os_waitqueue_p waitqueue)
{
    os_list_init(waitqueue -> waiting_list);
}