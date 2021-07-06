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

#include <dreamos.h>

static os_bool_t os_memory_initialized = OS_FALSE;

//内核内存系统初始化函数
void os_memory_init()
{
    os_memory_page_init();
    os_memory_slub_init();
    os_memory_initialized = OS_TRUE;
}

os_bool_t os_memory_is_initialized()
{
    return os_memory_initialized;
}

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

os_size_t os_get_allocated_memory()
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();
    return os_memory_page_get_allocated_page_count() * OS_MMU_PAGE_SIZE;
}

os_size_t os_get_total_memory()
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();
    return os_memory_page_get_total_page_count() * OS_MMU_PAGE_SIZE;
}

os_size_t os_get_free_memory()
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();
    return os_memory_page_get_free_page_count() * OS_MMU_PAGE_SIZE;
}