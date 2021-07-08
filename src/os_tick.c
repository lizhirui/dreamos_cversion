/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-07-06     lizhirui     add global tick
 */

// @formatter:off
#include <dreamos.h>

static os_size_t global_tick = 0;

//内核调度定时器中断处理程序
void os_tick_handler()
{
    global_tick++;
    os_task_t *cur_task = os_task_get_current_task();

    if(cur_task -> tick_remaining > 0)
    {
        cur_task -> tick_remaining--;
    }
    else
    {
        cur_task -> tick_remaining = cur_task -> tick_init;
        os_task_schedule();
    }
}

os_size_t os_tick_get()
{
    return global_tick;
}
