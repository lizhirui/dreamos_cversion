/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-06-02     lizhirui     add slub interface support
 */

// @formatter:off
#include <dreamos.h>

//标识内存子系统是否已经初始化完成
static os_bool_t os_memory_initialized = OS_FALSE;

/*!
 * 内存子系统初始化函数
 */
void os_memory_init()
{
    os_memory_page_init();
    os_memory_slub_init();
    os_memory_initialized = OS_TRUE;
}

/*!
 * 指示内存子系统是否已初始化完成
 * @return 若初始化完成，则返回OS_TRUE，否则返回OS_FALSE
 */
os_bool_t os_memory_is_initialized()
{
    return os_memory_initialized;
}

/*!
 * 分配指定大小的内存
 * @param size 内存大小
 * @return 成功返回内存地址，失败返回OS_NULL
 */
void *os_memory_alloc(os_size_t size)
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();
    
    void *ret;

    OS_ENTER_CRITICAL_AREA();

    //slub最大只能分配页面大小一半的对象
    if(size < (OS_MMU_PAGE_SIZE >> 1))
    {
        ret = os_memory_slub_alloc(size);
    }
    else
    {
        ret = os_memory_page_alloc(size);
    }

    OS_LEAVE_CRITICAL_AREA();

    if(ret != OS_NULL)
    {
        os_memset(ret,0,size);
    }

    return ret;
}

/*!
 * 释放内存
 * @param mem 要释放的内存地址
 */
void os_memory_free(void *mem)
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();
    OS_ENTER_CRITICAL_AREA();

    //判断地址是否和PAGE边界对齐，SLUB分配的对象地址永远都不会和PAGE边界对齐，反之buddy system分配的页面地址永远都和PAGE边界对齐
    if(CHECK_ALIGN((os_size_t)mem,PAGE_BITS))
    {
        os_memory_page_free(mem);
    }
    else
    {
        os_memory_slub_free(mem);
    }

    OS_LEAVE_CRITICAL_AREA();
}

/*!
 * 获取已分配的内存大小，获取的是buddy system已经分配的页面数，不考虑slub中的缓存空闲页面
 * @return 已分配的内存大小
 */
os_size_t os_get_allocated_memory()
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();
    return os_memory_page_get_allocated_page_count() * OS_MMU_PAGE_SIZE;
}

/*!
 * 获取总内存大小
 * @return 总内存大小
 */
os_size_t os_get_total_memory()
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();
    return os_memory_page_get_total_page_count() * OS_MMU_PAGE_SIZE;
}

/*!
 * 获取空闲内存大小
 * @return 空闲内存大小
 */
os_size_t os_get_free_memory()
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();
    return os_memory_page_get_free_page_count() * OS_MMU_PAGE_SIZE;
}