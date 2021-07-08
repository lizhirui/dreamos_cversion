// @formatter:off
#ifndef __OSCONFIG_H__
#define __OSCONFIG_H__

    #define OS_PRINTF_BUFFER_SIZE (256)
    #define PAGE_BITS (12)
    #define MEMORY_BASE (0x80000000UL)
    #define MEMORY_SIZE (128 * 0x100000)
    #define TASK_PRIORITY_MAX (31)
    #define TICK_PER_SECOND (100)
    #define IDLE_TASK_STACK_SIZE (8192)
    #define IDLE_TASK_TICK_INIT (10)
    #define MAIN_TASK_STACK_SIZE (16384)
    #define MAIN_TASK_PRIORITY (10)
    #define MAIN_TASK_TICK_INIT (1)
    #define OS_VFS_PATH_MAX (255)
    #define OS_TASK_MAX_NUM (65536)

    #define SLUB_MIN_PARTIAL (2)

    #define OS_MAX_OPEN_FILES (128)

    #define OS_ARCH64

#endif