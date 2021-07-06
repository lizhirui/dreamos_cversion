/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-02     lizhirui     the first version
 */

#ifndef __OS_MEMORY_SLUB_H__
#define __OS_MEMORY_SLUB_H__

    #include <dreamos.h>

    #define OS_MEMORY_SLUB_MIN_ORDER 3
    #define OS_MEMORY_SLUB_MAX_ORDER 11

    //以下两项不可改动
    #define OS_MEMORY_SLUB_SIZE OS_MMU_PAGE_SIZE
    #define OS_MEMORY_SLUB_BITS OS_MMU_OFFSET_BITS

    #define OS_MEMORY_SLUB_GET_OBJECT(x,offset) ((void *)ADDR_OFFSET((x),-(offset)))
    #define OS_MEMORY_SLUB_GET_OBJECT_METAINFO(x,offset) ((os_memory_slub_object_metainfo_p)ADDR_OFFSET((x),(offset)))

    typedef struct os_memory_slub_object_metainfo
    {
        struct os_memory_slub_object_metainfo *free_next;
    }os_memory_slub_object_metainfo_t,*os_memory_slub_object_metainfo_p;

    //前置声明，解决循环引用问题
    typedef struct os_memory_slub_cache *os_memory_slub_cache_p;

    typedef struct os_memory_slub_page
    {
        os_memory_slub_cache_p cache;
        os_size_t object_cur_nr;
        os_size_t object_total_nr;
        os_size_t object_zone_offset;
        os_memory_slub_object_metainfo_p free_item;
        struct os_memory_slub_page *prev;
        struct os_memory_slub_page *next;
    }os_memory_slub_page_t,*os_memory_slub_page_p;

    typedef struct os_memory_slub_cache
    {
        os_size_t object_size;
        os_size_t object_align_size;
        os_size_t object_total_size;
        os_size_t partial_nr;
        os_memory_slub_page_p partial;
    }os_memory_slub_cache_t,*os_memory_slub_cache_p;

    void os_memory_slub_init();
    void *os_memory_slub_alloc(os_size_t size);
    void os_memory_slub_free(void *object);

#endif