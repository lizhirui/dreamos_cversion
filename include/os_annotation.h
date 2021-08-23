/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-19     lizhirui     the first version
 */

// @formatter:off
#ifndef __OS_ANNOTATION_H__
#define __OS_ANNOTATION_H__

    #include <dreamos.h>

    void os_annotation_handler(const char *ex_string,const char *func,os_size_t line,const char *error_msg);

    //内核注解
    #define OS_ANNOTATION(condition,error_msg)                                                         \
    do                                                                                                 \
    {                                                                                                  \
        if (!(condition))                                                                              \
        {                                                                                              \
            os_annotation_handler(#condition,__FUNCTION__,__LINE__,error_msg);                         \
        }                                                                                              \
    }while(0)

    //以下注解用来保证内核的各个函数不在非法的环境下被调用
    //功能未实现
    #define OS_ANNOTATION_NOT_IMPLEMENT() OS_ANNOTATION(OS_FALSE,"This function isn't implemented!")
    //需要任务上下文
    #define OS_ANNOTATION_NEED_TASK_CONTEXT() OS_ANNOTATION((!os_is_in_interrupt()) && (os_task_scheduler_is_initialized()) && (os_task_get_current_task() != OS_NULL),"This function must be called in thread context!")
    //需要非中断上下文（不一定是任务上下文，还可能是启动第一个任务之前的初始上下文）
    #define OS_ANNOTATION_NEED_NON_INTERRUPT_CONTEXT() OS_ANNOTATION(!os_is_in_interrupt(),"This function can't be called in interrupt context!")
    //需要中断上下文
    #define OS_ANNOTATION_NEED_INTERRUPT_CONTEXT() OS_ANNOTATION(os_is_in_interrupt(),"This function must be called in interrupt context!")
    //需要内存子系统完成初始化
    #define OS_ANNOTATION_NEED_DYNAMIC_MEMORY() OS_ANNOTATION(os_memory_is_initialized(),"This function can't be called until dynamic memory system is initialized correctly!")
    //需要任务调度器完成初始化
    #define OS_ANNOTATION_NEED_TASK_SCHEDULER() OS_ANNOTATION(os_task_scheduler_is_initialized(),"This function can't be called until task scheduler is initialized correctly!")
    //需要MMU子系统完成初始化
    #define OS_ANNOTATION_NEED_MMU() OS_ANNOTATION(os_mmu_is_initialized(),"This function can't be called until mmu is initialized correctly!")
    //需要MMU完成预初始化
    #define OS_ANNOTATION_NEED_MMU_PREINIT() OS_ANNOTATION(os_mmu_is_preinitialized(),"This function can't be called until mmu is preinitialized correctly!")
    //需要VFS完成初始化
    #define OS_ANNOTATION_NEED_VFS() OS_ANNOTATION(os_vfs_is_initialized(),"This function can't be called until vfs is initialized correctly!")
    //需要设备管理器完成初始化
    #define OS_ANNOTATION_NEED_DEVICE() OS_ANNOTATION(os_device_is_initialized(),"This function can't be called until device is initialized correctly!")

#endif