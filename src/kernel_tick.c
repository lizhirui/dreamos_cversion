/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#include <dreamos.h>

void kernel_tick()
{
    task_t *cur_task = get_current_task();

    if(cur_task -> tick_remaining > 0)
    {
        cur_task -> tick_remaining--;
    }
    else
    {
        cur_task -> tick_remaining = cur_task -> tick_init;
        task_schedule();
    }
}