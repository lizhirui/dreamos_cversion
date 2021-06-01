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

//用于进入内核时的初始化
void bsp_early_init()
{
    tick_init();
}

//用于完成堆分配后的初始化
void bsp_after_heap_init()
{

}

//用于调度器完成初始化之后的初始化
void bsp_after_task_scheduler_init()
{
    os_printf("bsp_after_task_scheduler_init\n");
    //asm volatile("ebreak");
}

//用于向控制台打印字符串
void bsp_puts(const char *str)
{
    while(*str)
    {
        sbi_console_putchar(*str++);
    }
}
