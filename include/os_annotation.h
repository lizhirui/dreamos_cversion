#ifndef __OS_ANNOTATION_H__
#define __OS_ANNOTATION_H__

    #include <dreamos.h>

    void os_annotation_handler(const char *ex_string,const char *func,os_size_t line,const char *error_msg);

    #define OS_ANNOTATION(condition,error_msg)                                                         \
    do                                                                                                 \
    {                                                                                                  \
        if (!(condition))                                                                              \
        {                                                                                              \
            os_annotation_handler(#condition,__FUNCTION__,__LINE__,error_msg);                         \
        }                                                                                              \
    }while(0)

    #define OS_ANNOTATION_NOT_IMPLEMENT() OS_ANNOTATION(OS_FALSE,"This function isn't implemented!")
    #define OS_ANNOTATION_NEED_THREAD_CONTEXT() OS_ANNOTATION((!os_is_in_interrupt()) && (os_task_scheduler_is_initialized()) && (os_task_get_current_task() != OS_NULL),"This function must be called in thread context!")
    #define OS_ANNOTATION_NEED_NON_INTERRUPT_CONTEXT() OS_ANNOTATION(!os_is_in_interrupt(),"This function can't be called in interrupt context!")
    #define OS_ANNOTATION_NEED_INTERRUPT_CONTEXT() OS_ANNOTATION(os_is_in_interrupt(),"This function must be called in interrupt context!")
    #define OS_ANNOTATION_NEED_DYNAMIC_MEMORY() OS_ANNOTATION(os_memory_is_initialized(),"This function can't be called until dynamic memory system is initialized correctly!")
    #define OS_ANNOTATION_NEED_TASK_SCHEDULER() OS_ANNOTATION(os_task_scheduler_is_initialized(),"This function can't be called until task scheduler is initialized correctly!")
    #define OS_ANNOTATION_NEED_MMU() OS_ANNOTATION(os_mmu_is_initialized(),"This function can't be called until mmu is initialized correctly!")
    #define OS_ANNOTATION_NEED_MMU_PREINIT() OS_ANNOTATION(os_mmu_is_preinitialized(),"This function can't be called until mmu is preinitialized correctly!")
    #define OS_ANNOTATION_NEED_VFS() OS_ANNOTATION(os_vfs_is_initialized(),"This function can't be called until vfs is initialized correctly!")
    #define OS_ANNOTATION_NEED_DEVICE() OS_ANNOTATION(os_device_is_initialized(),"This function can't be called until device is initialized correctly!")

#endif