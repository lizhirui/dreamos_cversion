#ifndef __OSCONFIG_H__
#define __OSCONFIG_H__

    #define OS_PRINTF_BUFFER_SIZE (256)
    #define PAGE_SIZE (4096)
    #define PAGE_BITS (12)
    #define MEMORY_BASE (0x80000000)
    #define MEMORY_SIZE (128 * 0x100000)
    #define TASK_PRIORITY_MAX (31)
    #define TICK_PER_SECOND (100)
    #define IDLE_TASK_STACK_SIZE (8192)
    #define IDLE_TASK_TICK_INIT (10)
    #define MAIN_TASK_STACK_SIZE (16384)
    #define MAIN_TASK_PRIORITY (10)
    #define MAIN_TASK_TICK_INIT (1)

#endif