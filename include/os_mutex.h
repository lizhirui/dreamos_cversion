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
#ifndef __OS_MUTEX_H__
#define __OS_MUTEX_H__

    #include <dreamos.h>

    //互斥锁结构体
    typedef struct os_mutex
    {
        os_task_p owner;//拥有者
        os_size_t refcnt;//引用数，用于支持拥有者的递归调用
        os_bool_t locked;//锁定状态
        os_waitqueue_t waitqueue;//关联的等待队列
    }os_mutex_t,*os_mutex_p;

    void os_mutex_init(os_mutex_p mutex);
    void os_mutex_lock(os_mutex_p mutex);
    void os_mutex_unlock(os_mutex_p mutex);

#endif