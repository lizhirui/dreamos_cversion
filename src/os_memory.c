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

//内核内存系统初始化函数
void os_memory_init()
{
    phypage_init();
    slub_init();
}

void *os_memory_alloc(os_size_t size)
{
    //slub最大只能分配页面大小一半的对象
    if(size < (PAGE_SIZE >> 1))
    {
        return slub_alloc(size);
    }
    else
    {
        return phypage_alloc(size);
    }
}

void os_memory_free(void *mem)
{
    //判断地址是否和PAGE边界对齐，SLUB分配的对象地址永远都不会和PAGE边界对齐，反之buddy system分配的页面地址永远都和PAGE边界对齐
    if(CHECK_ALIGN((os_size_t)mem,PAGE_BITS))
    {
        phypage_free(mem);
    }
    else
    {
        slub_free(mem);
    }
}

os_size_t get_allocated_memory()
{
    return get_allocated_page_count() * PAGE_SIZE;
}

os_size_t get_total_memory()
{
    return get_total_page_count() * PAGE_SIZE;
}

os_size_t get_free_memory()
{
    return get_free_page_count() * PAGE_SIZE;
}