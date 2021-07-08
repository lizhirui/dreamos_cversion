/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-04     lizhirui     the first version
 * 2021-07-05     lizhirui     add io mapping support
 */

// @formatter:off
#include <dreamos.h>

static os_bool_t os_mmu_initialized = OS_FALSE;

void *os_mmu_create_io_mapping(os_mmu_vtable_p vtable,os_size_t pa,os_size_t size)
{
    OS_ASSERT(vtable != OS_NULL);
    //OS_ASSERT(vtable -> bitmap_initialized == OS_TRUE);
    
    //os_size_t va = os_bitmap_find_some_ones(&vtable -> va_bitmap,(OS_MMU_GET_EFFECTIVE_VA(OS_MMU_MEMORYMAP_IO_START) - vtable -> va_start) >> OS_MMU_OFFSET_BITS,size >> OS_MMU_OFFSET_BITS);
    os_size_t va = os_mmu_find_vaddr(vtable,OS_MMU_MEMORYMAP_IO_START,size);

    //if(va == OS_NUMBER_MAX(os_size_t))
    if(va == 0)
    {
        return OS_NULL;
    }

    //os_bitmap_set_bits(&vtable -> va_bitmap,va,size >> OS_MMU_OFFSET_BITS,0);
    //va = OS_MMU_GET_REAL_VA(vtable -> va_start + (va << OS_MMU_OFFSET_BITS));
    os_mmu_create_mapping(vtable,va,pa,size,OS_MMU_PROT_IO);
    return (void *)va;
}

void os_mmu_remove_io_mapping(os_mmu_vtable_p vtable,void *va,os_size_t size)
{
    OS_ASSERT(vtable != OS_NULL);
    //OS_ASSERT(vtable -> bitmap_initialized == OS_TRUE);
    
    //os_size_t va_n = OS_MMU_GET_EFFECTIVE_VA((os_size_t)va);
    os_size_t va_n = (os_size_t)va;

    os_mmu_remove_mapping(vtable,va_n,size);
    //va_n = (va_n - vtable -> va_start) >> OS_MMU_OFFSET_BITS;
    //os_bitmap_set_bits(&vtable -> va_bitmap,va_n,size >> OS_MMU_OFFSET_BITS,1);
}

os_err_t os_mmu_create_mapping_auto(os_mmu_vtable_p vtable,os_size_t va,os_size_t size,os_mmu_pt_prot_t prot)
{
    size = ALIGN_UP(size,OS_MMU_PAGE_SIZE);
    va = ALIGN_DOWN(va,OS_MMU_PAGE_SIZE);
    os_err_t ret;

    while(size)
    {
        void *mem = os_memory_alloc(OS_MMU_PAGE_SIZE);
        OS_ERR_RETURN_ERROR(mem == OS_NULL,-OS_ERR_ENOMEM);
        os_size_t pa = OS_MMU_VA_TO_PA((os_size_t)mem);

        if((ret = os_mmu_create_mapping(vtable,va,pa,size,prot)) != OS_ERR_OK)
        {
            os_memory_free(mem);
            return ret;
        }

        size -= OS_MMU_PAGE_SIZE;
    }

    return OS_ERR_OK;
}

os_bool_t os_mmu_is_initialized()
{
    return os_mmu_initialized;
}

os_err_t os_mmu_vtable_create(os_mmu_vtable_p vtable,os_mmu_pt_l1_p l1_vtable,os_size_t va_start,os_size_t va_size)
{
    OS_ASSERT(vtable != OS_NULL);
    OS_ASSERT(va_size != 0);

    vtable -> allocated = l1_vtable == OS_NULL;
    vtable -> va_start = va_start;
    vtable -> va_size = va_size;
    vtable -> refcnt = 0;
    //vtable -> bitmap_initialized = OS_FALSE;

    if(vtable -> allocated)
    {
        vtable -> l1_vtable = os_memory_alloc(OS_MMU_L1_PAGES * OS_MMU_PAGE_SIZE);

        if(vtable -> l1_vtable == OS_NULL)
        {
            os_memset(vtable,0,sizeof(*vtable));
            return -OS_ERR_ENOMEM;
        }
    }
    else
    {
        vtable -> l1_vtable = l1_vtable;
    }

    os_memset((void *)vtable -> l1_vtable,0,OS_MMU_L1_PAGES * OS_MMU_PAGE_SIZE);
    return OS_ERR_OK;
}

/*
void os_mmu_vtable_bitmap_init(os_mmu_vtable_p vtable,void *memory)
{
    OS_ASSERT(vtable != OS_NULL);
    OS_ASSERT(vtable -> bitmap_initialized == OS_FALSE);
    os_bitmap_create(&vtable -> va_bitmap,vtable -> va_size >> OS_MMU_OFFSET_BITS,memory,1);
    vtable -> bitmap_initialized = OS_TRUE;
}*/

void os_mmu_vtable_remove(os_mmu_vtable_p vtable,os_bool_t remove_mapping)
{
    OS_ASSERT(vtable != OS_NULL);

    /*
    if(vtable -> bitmap_initialized)
    {
        os_bitmap_remove(&vtable -> va_bitmap);
        vtable -> bitmap_initialized = OS_FALSE;
    }*/

    if(remove_mapping)
    {
        os_mmu_remove_all_mapping(vtable);
    }

    if(vtable -> allocated)
    {
        os_memory_free((void *)vtable -> l1_vtable);
        vtable -> l1_vtable = OS_NULL;
        vtable -> allocated = OS_FALSE;
    }

    vtable -> va_start = 0;
    vtable -> va_size = 0;
}

void arch_mmu_switch(os_mmu_vtable_p vtable);

static os_mmu_vtable_p current_vtable = OS_NULL;

void os_mmu_switch(os_mmu_vtable_p vtable)
{
    OS_ASSERT(vtable != OS_NULL);
    
    if(current_vtable != vtable)
    {
        current_vtable = vtable;
        arch_mmu_switch(current_vtable);
    }
}

os_mmu_vtable_p os_mmu_get_current_vtable()
{
    return current_vtable;
}

os_bool_t os_mmu_io_mapping_copy(os_mmu_vtable_p vtable);

os_bool_t os_mmu_page_fault_handler(os_size_t addr,os_bool_t write)
{
    if(addr >= OS_MMU_MEMORYMAP_IO_START)
    {
        return os_mmu_io_mapping_copy(current_vtable);
    }

    return OS_FALSE;
}

void os_mmu_init()
{
    //os_mmu_vtable_bitmap_init(os_mmu_get_kernel_pagetable(),OS_NULL);
    os_mmu_initialized = OS_TRUE;
}