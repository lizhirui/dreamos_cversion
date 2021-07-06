/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-02     lizhirui     the first version
 * 2021-07-06     lizhirui     fix a slub_page_init bug that page -> object_total_nr is wrong
 */

#include <dreamos.h>

static os_memory_slub_cache_t os_memory_slub_cache[OS_MEMORY_SLUB_MAX_ORDER + 1]; 

static void slub_test()
{
    os_size_t i;
    os_printf("slub test\n");

    for(i = 1;i <= (OS_MMU_PAGE_SIZE >> 1);i <<= 1)
    {
        os_printf("size = %d\n",i);
        os_size_t order = MAX(ALIGN_UP_MIN(i),OS_MEMORY_SLUB_MIN_ORDER);
        os_memory_slub_cache_p cache = &os_memory_slub_cache[order];
        void *mem1 = os_memory_slub_alloc(i);
        os_printf("mem1 = 0x%p\n",mem1);
        void *mem2 = os_memory_slub_alloc(i);
        os_printf("mem2 = 0x%p\n",mem2);
        os_memory_slub_page_p page = cache -> partial;
        OS_ASSERT((((os_size_t)mem1) & MASK(OS_MEMORY_SLUB_BITS)) == (ALIGN_UP(sizeof(os_memory_slub_page_t),os_memory_slub_cache[order].object_align_size)));
        OS_ASSERT(((((os_size_t)mem1) & MASK(OS_MEMORY_SLUB_BITS)) == (ALIGN_UP(sizeof(os_memory_slub_page_t),os_memory_slub_cache[order].object_align_size))) || ((((os_size_t)mem2) & MASK(OS_MEMORY_SLUB_BITS)) == (ALIGN_UP(sizeof(os_memory_slub_page_t),os_memory_slub_cache[order].object_align_size) + (ALIGN_UP((1 << order) + sizeof(os_memory_slub_object_metainfo_t),os_memory_slub_cache[i].object_align_size)))));
        os_memory_slub_free(mem1);
        os_memory_slub_free(mem2);
        void *mem3 = os_memory_slub_alloc(i);
        os_printf("mem3 = 0x%p\n",mem1);
        void *mem4 = os_memory_slub_alloc(i);
        os_printf("mem4 = 0x%p\n",mem2);
        OS_ASSERT(mem2 == mem3);
        OS_ASSERT(mem1 == mem4);
        os_memory_slub_free(mem3);
        os_memory_slub_free(mem4);
    }
}

void os_memory_slub_init()
{
    size_t i;

    OS_ASSERT((1 << OS_MEMORY_SLUB_BITS) == OS_MEMORY_SLUB_SIZE);

    for(i = OS_MEMORY_SLUB_MIN_ORDER;i <= OS_MEMORY_SLUB_MAX_ORDER;i++)
    {
        os_memory_slub_cache[i].object_size = 1 << i;
        //os_memory_slub_cache[i].object_align_size = 1 << ALIGN_UP_MIN(os_memory_slub_cache[i].object_size + sizeof(os_memory_slub_object_metainfo_t));
        os_memory_slub_cache[i].object_align_size = sizeof(os_size_t);
        os_memory_slub_cache[i].object_total_size = ALIGN_UP(os_memory_slub_cache[i].object_size + sizeof(os_memory_slub_object_metainfo_t),os_memory_slub_cache[i].object_align_size);
        os_memory_slub_cache[i].partial_nr = 0;
        os_memory_slub_cache[i].partial = OS_NULL;
    }
    
    //slub_test();
    //slub_test();
}

//初始化空闲链表
static void slub_page_init(os_memory_slub_page_p page)
{
    //初始化页面数据结构
    page -> object_zone_offset = ALIGN_UP(sizeof(os_memory_slub_page_t),page -> cache -> object_align_size);
    page -> object_total_nr = (OS_MEMORY_SLUB_SIZE - page -> object_zone_offset) / page -> cache -> object_total_size;
    page -> object_cur_nr = page -> object_total_nr;

    OS_ASSERT(page -> object_total_nr > 0);

    void *page_object_zone = ADDR_OFFSET(page,page -> object_zone_offset);

    //让第0项为默认空闲项
    page -> free_item = OS_MEMORY_SLUB_GET_OBJECT_METAINFO(page_object_zone,page -> cache -> object_size);
    
    //遍历其余项，形成一条单链表
    os_size_t offset = 0;
    os_size_t i;

    for(i = 1;i <= page -> object_total_nr;offset += page -> cache -> object_total_size,i++)
    {
        os_memory_slub_object_metainfo_p cur_object = OS_MEMORY_SLUB_GET_OBJECT_METAINFO(ADDR_OFFSET(page_object_zone,offset),page -> cache -> object_size);
        
        //链表终结判定
        if(i < page -> object_total_nr)
        {
            os_memory_slub_object_metainfo_p next_object = OS_MEMORY_SLUB_GET_OBJECT_METAINFO(ADDR_OFFSET(page_object_zone,offset + page -> cache -> object_total_size),page -> cache -> object_size);
            cur_object -> free_next = next_object;
        }
        else
        {
            cur_object -> free_next = OS_NULL;
        }
    }
}

//扩增slub，分配最多SLUB_MIN_PARTIAL项
static void slub_expand(os_memory_slub_cache_p cache)
{
    os_size_t i;

    cache -> partial = OS_NULL;
    cache -> partial_nr = 0;

    for(i = 0;i < SLUB_MIN_PARTIAL;i++)
    {
        os_memory_slub_page_p new_page = (os_memory_slub_page_p)os_memory_page_alloc(OS_MEMORY_SLUB_SIZE);

        if(new_page != OS_NULL)
        {
            //增加部分slub数
            cache -> partial_nr++;
            //将新slub挂入链表
            new_page -> next = cache -> partial;
            cache -> partial = new_page;
            new_page -> cache = cache;
            new_page -> prev = OS_NULL;

            if(new_page -> next != OS_NULL)
            {
                new_page -> next -> prev = new_page;
            }

            //初始化空闲链表
            slub_page_init(new_page);
        }
        else
        {
            break;
        }

        //溢出保护
        if(cache -> partial_nr == SIZE_MAX)
        {
            break;
        }
    }
}

void *os_memory_slub_alloc(os_size_t size)
{
    os_size_t order = MAX(ALIGN_UP_MIN(size),OS_MEMORY_SLUB_MIN_ORDER);
    os_size_t object_size = 1 << order;
    os_size_t object_item_size = object_size + sizeof(os_memory_slub_object_metainfo_t);
    os_memory_slub_cache_p cache = &os_memory_slub_cache[order];
    os_size_t i;
    
    //检测是否无可用SLUB
    if(cache -> partial_nr == 0)
    {
        //扩增slub，分配最多SLUB_MIN_PARTIAL项
        slub_expand(cache);
    }

    //再次检测是否无可用SLUB
    if(cache -> partial_nr == 0)
    {
        return OS_NULL;//无可用页面
    }
    
    //分配新对象
    os_memory_slub_object_metainfo_p new_object_metainfo = cache -> partial -> free_item;
    cache -> partial -> object_cur_nr--;
    OS_ASSERT(cache -> partial -> object_cur_nr <= cache -> partial -> object_total_nr);

    os_memory_slub_page_p old_page = cache -> partial;

    //从slub中移除该项
    old_page -> free_item = new_object_metainfo -> free_next;

    //判定该对象是否为该slub中的最后一个空闲项
    if(new_object_metainfo -> free_next == OS_NULL)
    {
        //从半空slub链表中移除该slub
        old_page -> free_item = OS_NULL;
        cache -> partial = old_page -> next;

        if(cache -> partial != OS_NULL)
        {
            cache -> partial -> prev = OS_NULL;
        }

        cache -> partial_nr--;
        old_page -> next = OS_NULL;
        old_page -> prev = OS_NULL;
    }
    else
    {
        new_object_metainfo -> free_next = OS_NULL;
    }

    //返回分配的对象
    return OS_MEMORY_SLUB_GET_OBJECT(new_object_metainfo,cache -> object_size);
}

void os_memory_slub_free(void *object)
{
    //获得该object对应的slub
    os_memory_slub_page_p page = (os_memory_slub_page_p)UMASK_VALUE((os_size_t)object,MASK(OS_MEMORY_SLUB_BITS));

    //判断该slub是否是个满slub
    if(page -> free_item == OS_NULL)
    {
        //将该slub挂入对应cache的半空slub链表中
        page -> next = page -> cache -> partial;
        page -> cache -> partial = page;
        page -> cache -> partial_nr++;

        if(page -> next != OS_NULL)
        {
            page -> next -> prev = page;
        }

        page -> prev = OS_NULL;
    }

    //将该object归还给slub
    os_memory_slub_object_metainfo_p object_metainfo = OS_MEMORY_SLUB_GET_OBJECT_METAINFO(object,page -> cache -> object_size);
    object_metainfo -> free_next = page -> free_item;
    page -> free_item = object_metainfo;
    page -> object_cur_nr++;
    OS_ASSERT(page -> object_cur_nr <= page -> object_total_nr);

    //判断该slub是否为空并判断半空slub数是否大于SLUB_MIN_PARTIAL项，最后一部分是溢出保护
    if(((page -> object_cur_nr == page -> object_total_nr) && (page -> cache -> partial_nr > SLUB_MIN_PARTIAL)) || (page -> cache -> partial_nr == SIZE_MAX))
    {
        //将该slub从cache的半空slub链中移除
        if(page -> next != OS_NULL)
        {
            page -> next -> prev = page -> prev;
        }

        if(page -> prev != OS_NULL)
        {
            page -> prev -> next = page -> next;
        }
        else
        {
            page -> cache -> partial = page -> next;
        }

        //将该slub归还到buddy system
        os_memory_page_free(page);
    }
}