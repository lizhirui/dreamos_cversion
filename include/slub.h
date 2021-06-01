#ifndef __SLUB_H__
#define __SLUB_H__

    #include <dreamos.h>

    #define SLUB_MIN_ORDER 3
    #define SLUB_MAX_ORDER 11

    //以下两项不可改动
    #define SLUB_SIZE PAGE_SIZE
    #define SLUB_BITS PAGE_BITS

    #define GET_SLUB_OBJECT(x,offset) ((void *)ADDR_OFFSET((x),-(offset)))
    #define GET_SLUB_OBJECT_METAINFO(x,offset) ((slub_object_metainfo_p)ADDR_OFFSET((x),(offset)))

    typedef struct slub_object_metainfo
    {
        struct slub_object_metainfo *free_next;
    }slub_object_metainfo_t,*slub_object_metainfo_p;

    //前置声明，解决循环引用问题
    typedef struct slub_cache *slub_cache_p;

    typedef struct slub_page
    {
        slub_cache_p cache;
        os_size_t object_cur_nr;
        os_size_t object_total_nr;
        os_size_t object_zone_offset;
        slub_object_metainfo_p free_item;
        struct slub_page *prev;
        struct slub_page *next;
    }slub_page_t,*slub_page_p;

    typedef struct slub_cache
    {
        os_size_t object_size;
        os_size_t object_align_size;
        os_size_t object_total_size;
        os_size_t partial_nr;
        slub_page_p partial;
    }slub_cache_t,*slub_cache_p;

    void slub_init();
    void *slub_alloc(os_size_t size);
    void slub_free(void *object);

#endif