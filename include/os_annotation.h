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

    #define OS_ANNOTATION_CANNOT_RUN_IN_INTERRUPT() OS_ANNOTATION(!os_is_in_interrupt(),"This function can't be called in interrupt context!")
    #define OS_ANNOTATION_MUST_RUN_IN_INTERRUPT() OS_ANNOTATION(os_is_in_interrupt(),"This function must be called in interrupt context!")

#endif