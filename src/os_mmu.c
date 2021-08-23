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

//表示MMU子系统是否已初始化完成
static os_bool_t os_mmu_initialized = OS_FALSE;

/*!
 * 创建IO Mapping
 * @param vtable 页表结构体指针
 * @param pa 要映射的物理地址
 * @param size 映射大小
 * @return 成功返回虚拟地址，失败返回OS_NULL
 */
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


/*!
 * 移除IO Mapping
 * @param vtable 页表
 * @param va 要映射的虚拟地址
 * @param size 映射大小
 */
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

/*!
 * 在指定虚拟地址处映射并自动分配物理内存空间
 * @param vtable 页表结构体指针
 * @param va 虚拟地址
 * @param size 内存大小
 * @param prot 内存属性
 * @return 成功返回OS_TRUE，失败返回负数错误码
 */
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

/*!
 * 指示MMU子系统是否已初始化完成
 * @return 若已初始化完成，则返回OS_TRUE，否则返回OS_FALSE
 */
os_bool_t os_mmu_is_initialized()
{
    return os_mmu_initialized;
}

/*!
 * 创建页表
 * @param vtable 页表结构体指针
 * @param l1_vtable l1页表指针，若为OS_NULL,则自动分配
 * @param va_start 起始虚拟地址
 * @param va_size 虚拟地址空间大小
 * @return 成功返回OS_ERR_OK，l1页表分配失败，返回-OS_ERR_ENOMEM
 */
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

/*!
 * 销毁页表
 * @param vtable 页表结构体指针
 * @param remove_mapping 是否同时遍历销毁其中的每个映射（除了MMU子系统刚启动内核跳入时为OS_FALSE时，一般情况下都为OS_TRUE）
 */
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

//当前页表结构体指针
static os_mmu_vtable_p current_vtable = OS_NULL;

/*!
 * 切换页表
 * @param vtable 页表结构体指针
 */
void os_mmu_switch(os_mmu_vtable_p vtable)
{
    OS_ASSERT(vtable != OS_NULL);
    
    if(current_vtable != vtable)
    {
        current_vtable = vtable;
        arch_mmu_switch(current_vtable);
    }
}

/*!
 * 获取当前的页表结构体指针
 * @return
 */
os_mmu_vtable_p os_mmu_get_current_vtable()
{
    return current_vtable;
}

os_bool_t os_mmu_io_mapping_copy(os_mmu_vtable_p vtable);

/*!
 * 页面fault处理程序
 * @param addr 出错地址
 * @param write 是否为写操作
 * @return 成功处理返回OS_TRUE，否则返回OS_FALSE
 */
os_bool_t os_mmu_page_fault_handler(os_size_t addr,os_bool_t write)
{
    if(addr >= OS_MMU_MEMORYMAP_IO_START)
    {
        return os_mmu_io_mapping_copy(current_vtable);
    }

    return OS_FALSE;
}

/*!
 * MMU子系统初始化
 */
void os_mmu_init()
{
    //os_mmu_vtable_bitmap_init(os_mmu_get_kernel_pagetable(),OS_NULL);
    os_mmu_initialized = OS_TRUE;
}