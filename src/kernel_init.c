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

static void print_logo()
{
    os_printf("\n");
    os_printf("************************************************\n");
    os_printf("   ____                            ___  ____\n");
    os_printf("  |  _ \\ _ __ ___  __ _ _ __ ___  / _ \\/ ___|\n");
    os_printf("  | | | | '__/ _ \\/ _` | '_ ` _ \\| | | \\___ \\\n");
    os_printf("  | |_| | | |  __/ (_| | | | | | | |_| |___) |\n");
    os_printf("  |____/|_|  \\___|\\__,_|_| |_| |_|\\___/|____/\n");
    os_printf("\n");
    os_printf("************************************************\n");
}

static void print_system_info()
{
    print_logo();
    os_printf("\n");
    os_printf("DreamOS 1.0\n");
    os_printf("Author: lizhirui <exiis@126.com>\n");
    os_printf("\n");
    os_printf("************************************************\n");
    os_printf("\n");
}

//内核初始化函数，也是内核入口
void kernel_init()
{
    bsp_early_init();
    print_system_info();
    os_memory_init();
    bsp_after_heap_init();
    task_scheduler_init();
    bsp_after_task_scheduler_init();
    task_scheduler_start();
    while(1);
}