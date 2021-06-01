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

typedef struct page_metainfo
{
    size_t order;
    struct page_metainfo *prev;
    struct page_metainfo *next;
}page_metainfo_t;

extern size_t _heap_start;

#define BUDDY_ORDER_MAX (sizeof(size_t) << 3)
#define BUDDY_ORDER_UPLIMIT (BUDDY_ORDER_MAX + 1)

static page_metainfo_t page_list[BUDDY_ORDER_UPLIMIT];

static size_t page_metainfo_start;
static size_t page_metainfo_end;
static size_t page_memory_start;
static size_t page_memory_end;
static size_t page_metainfo_bits_aligned;
static size_t page_allocated;

static page_metainfo_t *addr_to_page_metainfo(size_t addr)
{
    if(addr < page_memory_start)
    {
        return NULL;
    }

    size_t r = (((addr - page_memory_start) >> PAGE_BITS) << page_metainfo_bits_aligned) + page_metainfo_start;

    if(r >= page_metainfo_end)
    {
        return NULL;
    }

    return (page_metainfo_t *)r;
}

static size_t page_metainfo_to_addr(page_metainfo_t *page_metainfo)
{
    size_t r = (((((size_t)page_metainfo) - page_metainfo_start) >> page_metainfo_bits_aligned) << PAGE_BITS) + page_memory_start;

    if(r >= page_memory_end)
    {
        return 0;
    }

    return r;
}

static inline size_t size_to_order(size_t size)
{
    size_t pos = ALIGN_UP_MIN(size);

    if(pos < PAGE_BITS)
    {
        return PAGE_BITS;
    }
    else
    {
        return pos;
    }
}

static inline page_metainfo_t *buddy_get(page_metainfo_t *page,size_t order)
{
    addr_to_page_metainfo(page_metainfo_to_addr(page) ^ SIZE(order));
}

static void page_insert(size_t order,page_metainfo_t *page)
{
    page -> prev = &page_list[order];
    page -> next = page_list[order].next;
    page_list[order].next = page;
    page -> order = order;

    if(page -> next != NULL)
    {
        page -> next -> prev = page;
    }
}

static void page_remove(page_metainfo_t *page)
{
    page -> prev -> next = page -> next;

    if(page -> next != NULL)
    {
        page -> next -> prev = page -> prev;
    }

    page -> order = BUDDY_ORDER_UPLIMIT;
}

static inline page_metainfo_t *get_big_page(page_metainfo_t *page,page_metainfo_t *buddy)
{
    if(((size_t)page) <= ((size_t)buddy))
    {
        return page;
    }
    else
    {
        return buddy;
    }
}

static void *_alloc(size_t order)
{
    size_t i;
    ENTER_CRITICAL_AREA();

    for(i = order;i < BUDDY_ORDER_UPLIMIT;i++)
    {
        if(page_list[i].next != NULL)
        {
            page_metainfo_t *page = page_list[i].next;
            size_t addr = page_metainfo_to_addr(page);
            page_remove(page);

            while(i > order)
            {
                i--;
                size_t right_new_addr = addr + SIZE(i);
                page_metainfo_t *right_new_page = addr_to_page_metainfo(right_new_addr);
                page_insert(i,right_new_page);
            }

            page_allocated += SIZE(order - PAGE_BITS);
            SYNC_DATA();
            LEAVE_CRITICAL_AREA();
            return (void *)addr;
        }
    }

    SYNC_DATA();
    LEAVE_CRITICAL_AREA();
    return NULL;
}

//页面分配（其大小为2的幂，且>=size）
void *phypage_alloc(size_t size)
{
    return _alloc(size_to_order(size));
}

static void _free(void *addr,size_t old_order)
{
    ENTER_CRITICAL_AREA();
    page_metainfo_t *page = addr_to_page_metainfo((size_t)addr);
    size_t i;

    for(i = old_order;i < BUDDY_ORDER_UPLIMIT;i++)
    {
        page_metainfo_t *buddy = buddy_get(page,i);

        if(((buddy != NULL) && (buddy -> order == i) && (i < BUDDY_ORDER_MAX)))
        {
            page_remove(buddy);
            page = get_big_page(page,buddy);
        }
        else
        {
            page_allocated -= SIZE(old_order - PAGE_BITS);
            page_insert(i,page);
            break;
        }
    }

    SYNC_DATA();
    LEAVE_CRITICAL_AREA();
}

//页面释放
void phypage_free(void *addr,size_t old_size)
{
    _free(addr,size_to_order(old_size));
}

//获取已分配的页面数量
size_t get_allocated_page_count()
{
    return page_allocated;
}

//获取总页面数量
size_t get_total_page_count()
{
    return (page_memory_end - page_memory_start) >> PAGE_BITS;
}

//获取空闲页面数量
size_t get_free_page_count()
{
    return get_total_page_count() - get_allocated_page_count();
}

static void test()
{
    void *mem1 = phypage_alloc(131072);
    os_printf("mem1 = 0x%p\n",mem1);
    void *mem2 = phypage_alloc(4096);
    os_printf("mem2 = 0x%p\n",mem2);
    void *mem3 = phypage_alloc(4096);
    os_printf("mem3 = 0x%p\n",mem3);
    phypage_free(mem1,131072);
    void *mem4 = phypage_alloc(131072);
    os_printf("mem4 = 0x%p\n",mem4);
    phypage_free(mem2,4096);
    phypage_free(mem3,4096);
    phypage_free(mem4,131072);
    OS_ASSERT(page_allocated == 0);
}

//buddy system初始化函数
void phypage_init()
{
    size_t heap_start = (size_t)&_heap_start;
    size_t mem_start = ALIGN_UP(heap_start,PAGE_SIZE);
    size_t mem_end = ALIGN_DOWN(MEMORY_BASE + MEMORY_SIZE,PAGE_SIZE);
    size_t mem_size = mem_end - mem_start;
    os_printf("memory layout:\nmem_start = 0x%p\nmem_end = 0x%p\nmem_size = 0x%p\n",mem_start,mem_end,mem_size);

    size_t i;

    for(i = 0;i < BUDDY_ORDER_UPLIMIT;i++)
    {
        page_list[i].order = i;
        page_list[i].prev = NULL;
        page_list[i].next = NULL;
    }

    page_metainfo_bits_aligned = ALIGN_UP_MIN(sizeof(page_metainfo_t));
    size_t meta_size = SIZE(page_metainfo_bits_aligned);
    size_t page_size = PAGE_SIZE;
    size_t page_num = mem_size / (meta_size + page_size);
    page_metainfo_start = mem_start;
    page_metainfo_end = page_metainfo_start + (page_num << page_metainfo_bits_aligned);
    page_memory_start = ALIGN_UP(page_metainfo_end,PAGE_SIZE);
    page_memory_end = page_memory_start + (page_num << PAGE_BITS);
    os_printf("phypage layout:\nmeta_size = %ld\npage_size = %ld\npage_num = %ld\n",meta_size,page_size,page_num);
    os_printf("page_metainfo_start = 0x%p\npage_metainfo_end = 0x%p\npage_memory_start = 0x%p\npage_memory_end = 0x%p\n",page_metainfo_start,page_metainfo_end,page_memory_start,page_memory_end);

    for(i = 0;i < page_num;i++)
    {
        page_metainfo_t *page = (page_metainfo_t *)(page_metainfo_start + (i << page_metainfo_bits_aligned));
        page -> order = BUDDY_ORDER_UPLIMIT;
        page -> prev = NULL;
        page -> next = NULL;
    }

    page_allocated = page_num;
    
    size_t cur_page_addr = page_memory_start;

    while(cur_page_addr < page_memory_end)
    {
        size_t size_bits = ALIGN_DOWN_MAX(cur_page_addr);
        size_t align_bits = __builtin_ctzl(cur_page_addr);

        if(align_bits < size_bits)
        {
            size_bits = align_bits;
        }

        if(size_bits > BUDDY_ORDER_MAX)
        {
            size_bits = BUDDY_ORDER_MAX;
        }

        os_printf("page: 0x%p,size_bits: %ld\n",cur_page_addr,size_bits);
        OS_ASSERT(size_bits >= PAGE_BITS);
        _free((void *)cur_page_addr,size_bits);
        cur_page_addr += 1 << size_bits;
    }

    OS_ASSERT(page_allocated == 0);
    SYNC_DATA();
    test();
    test();
}