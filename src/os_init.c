/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

// @formatter:off
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

static inline void os_build_check()
{
    OS_BUILD_ASSERT(OS_SIZE_T_BITS != 0);
}

//内核初始化函数，也是内核入口
void os_init()
{
    os_build_check();
    bsp_early_init();
    print_system_info();
    os_memory_init();
    os_mmu_init();
    bsp_after_heap_init();
    os_vfs_init();
    os_task_scheduler_init();
    bsp_after_task_scheduler_init();
    os_task_scheduler_start();
    
    while(1);
}