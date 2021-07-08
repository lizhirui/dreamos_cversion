/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-06     lizhirui     the first version
 */

// @formatter:off
#ifndef __OS_TICK_H__
#define __OS_TICK_H__

    #include <dreamos.h>

    os_size_t os_tick_get();
    void os_tick_handler();

#endif