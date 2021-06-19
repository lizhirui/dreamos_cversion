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

extern task_t task_test;

os_ssize_t task_main_entry(os_size_t arg)
{
    while(1)
    {
        os_printf("task_main_entry\n");
        task_wakeup(&task_test);
    }
}