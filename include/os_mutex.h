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

    typedef struct os_mutex
    {
        os_task_p owner;
        os_size_t refcnt;
        os_bool_t locked;
        os_waitqueue_t waitqueue;
    }os_mutex_t,*os_mutex_p;

    void os_mutex_init(os_mutex_p mutex);
    void os_mutex_lock(os_mutex_p mutex);
    void os_mutex_unlock(os_mutex_p mutex);

#endif