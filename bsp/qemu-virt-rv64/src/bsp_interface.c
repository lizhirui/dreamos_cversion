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
#include <sbi.h>

void bsp_early_init()
{
    tick_init();
}

void bsp_after_heap_init()
{

}

void bsp_after_task_scheduler_init()
{
    os_printf("bsp_after_task_scheduler_init\n");
    //asm volatile("ebreak");
}

void bsp_puts(const char *str)
{
    while(*str)
    {
        sbi_console_putchar(*str++);
    }
}
