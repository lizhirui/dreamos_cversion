/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

// @formatter:off
#ifndef __OS_MMU_H__
#define __OS_MMU_H__

    #include <dreamos.h>
    #include <arch.h>

    //下面的声明是用来支持最大4级页表的

    #if (OS_MMU_PAGETABLE_LEVEL_NUM > 4UL) || (OS_MMU_PAGETABLE_LEVEL_NUM < 1UL)
        #error "DreamOS only support 1~4 level pagetable!"
    #endif

    #define OS_MMU_PAGE_SIZE SIZE(OS_MMU_OFFSET_BITS)
    #define OS_MMU_PAGE_OFFSET(addr) MASK_VALUE(addr,MASK(OS_MMU_OFFSET_BITS))

    #if OS_MMU_PAGETABLE_LEVEL_NUM == 4
        #define OS_MMU_L4_SHIFT OS_MMU_OFFSET_BITS
        #define OS_MMU_L4_SIZE SIZE(OS_MMU_L4_SHIFT)
        #define OS_MMU_L4_ENTRY_NUM SIZE(OS_MMU_L4_BITS)
        #define OS_MMU_L4_MASK (MASK(OS_MMU_L4_BITS) << OS_MMU_L4_SHIFT)
        #define OS_MMU_L4_ID(va) (((va) & OS_MMU_L4_MASK) >> OS_MMU_L4_SHIFT)
        #define OS_MMU_L4_PAGES DIV_UP(OS_MMU_L4_ENTRY_NUM * sizeof(os_mmu_pt_l4_t),OS_MMU_PAGE_SIZE)
        #define OS_MMU_L4_CHECK_ALIGN(va) (((va) & MASK(OS_MMU_L4_SHIFT)) == 0)
        #define OS_MMU_L4_ALIGN(va) (((va) & UMASK(OS_MMU_L4_SHIFT)))
        #define OS_MMU_L4_OFFSET(va) (((va) & MASK(OS_MMU_L4_SHIFT)))
        typedef os_mmu_pt_l4_t *os_mmu_pt_l4_p;
    #else
        #define OS_MMU_L4_SHIFT OS_MMU_OFFSET_BITS
        #define OS_MMU_L4_BITS 0
    #endif
    
    #if OS_MMU_PAGETABLE_LEVEL_NUM >= 3
        #define OS_MMU_L3_SHIFT (OS_MMU_L4_SHIFT + OS_MMU_L4_BITS)
        #define OS_MMU_L3_SIZE SIZE(OS_MMU_L3_SHIFT)
        #define OS_MMU_L3_ENTRY_NUM SIZE(OS_MMU_L3_BITS)
        #define OS_MMU_L3_MASK (MASK(OS_MMU_L3_BITS) << OS_MMU_L3_SHIFT)
        #define OS_MMU_L3_ID(va) (((va) & OS_MMU_L3_MASK) >> OS_MMU_L3_SHIFT)
        #define OS_MMU_L3_PAGES DIV_UP(OS_MMU_L3_ENTRY_NUM * sizeof(os_mmu_pt_l3_t),OS_MMU_PAGE_SIZE)
        #define OS_MMU_L3_CHECK_ALIGN(va) (((va) & MASK(OS_MMU_L3_SHIFT)) == 0)
        #define OS_MMU_L3_ALIGN(va) (((va) & UMASK(OS_MMU_L3_SHIFT)))
        #define OS_MMU_L3_OFFSET(va) (((va) & MASK(OS_MMU_L3_SHIFT)))
        typedef os_mmu_pt_l3_t *os_mmu_pt_l3_p;
    #else
        #define OS_MMU_L3_SHIFT OS_MMU_OFFSET_BITS
        #define OS_MMU_L3_BITS 0
    #endif

    #if OS_MMU_PAGETABLE_LEVEL_NUM >= 2
        #define OS_MMU_L2_SHIFT (OS_MMU_L3_SHIFT + OS_MMU_L3_BITS)
        #define OS_MMU_L2_SIZE SIZE(OS_MMU_L2_SHIFT)
        #define OS_MMU_L2_ENTRY_NUM SIZE(OS_MMU_L2_BITS)
        #define OS_MMU_L2_MASK (MASK(OS_MMU_L2_BITS) << OS_MMU_L2_SHIFT)
        #define OS_MMU_L2_ID(va) (((va) & OS_MMU_L2_MASK) >> OS_MMU_L2_SHIFT)
        #define OS_MMU_L2_PAGES DIV_UP(OS_MMU_L2_ENTRY_NUM * sizeof(os_mmu_pt_l2_t),OS_MMU_PAGE_SIZE)
        #define OS_MMU_L2_CHECK_ALIGN(va) (((va) & MASK(OS_MMU_L2_SHIFT)) == 0)
        #define OS_MMU_L2_ALIGN(va) (((va) & UMASK(OS_MMU_L2_SHIFT)))
        #define OS_MMU_L2_OFFSET(va) (((va) & MASK(OS_MMU_L2_SHIFT)))
        typedef os_mmu_pt_l2_t *os_mmu_pt_l2_p;
    #else
        #define OS_MMU_L2_SHIFT OS_MMU_OFFSET_BITS
        #define OS_MMU_L2_BITS 0
    #endif

    //L1地址起始位
    #define OS_MMU_L1_SHIFT (OS_MMU_L2_SHIFT + OS_MMU_L2_BITS)
    //L1页表每项对应的虚拟地址空间大小
    #define OS_MMU_L1_SIZE SIZE(OS_MMU_L1_SHIFT)
    //L1页表可存放的表项数
    #define OS_MMU_L1_ENTRY_NUM SIZE(OS_MMU_L1_BITS)
    //用于通过与操作截取虚拟地址中的L1表项索引部分
    #define OS_MMU_L1_MASK (MASK(OS_MMU_L1_BITS) << OS_MMU_L1_SHIFT)
    //用于截取虚拟地址中的L1表项索引部分
    #define OS_MMU_L1_ID(va) (((va) & OS_MMU_L1_MASK) >> OS_MMU_L1_SHIFT)
    //L1页表所占的页面数
    #define OS_MMU_L1_PAGES DIV_UP(OS_MMU_L1_ENTRY_NUM * sizeof(os_mmu_pt_l1_t),OS_MMU_PAGE_SIZE)
    //检查地址有没有对齐到L1页表对应的虚拟地址边界
    #define OS_MMU_L1_CHECK_ALIGN(va) (((va) & MASK(OS_MMU_L1_SHIFT)) == 0)
    //将地址对齐到L1页表对应的虚拟地址边界
    #define OS_MMU_L1_ALIGN(va) (((va) & UMASK(OS_MMU_L1_SHIFT)))
    //获取虚拟地址相对于L1页表的偏移部分
    #define OS_MMU_L1_OFFSET(va) (((va) & MASK(OS_MMU_L1_SHIFT)))
    typedef os_mmu_pt_l1_t *os_mmu_pt_l1_p;

    //以下两个宏用于物理地址和物理页面号互转
    #define OS_MMU_PA_TO_PPN(pa) ((pa) >> OS_MMU_OFFSET_BITS)
    #define OS_MMU_PPN_TO_PA(ppn) ((ppn) << OS_MMU_OFFSET_BITS)

    //以下宏保证用户空间/内核空间和IO映射空间的顺序，即从低地址到高地址必须是用户空间、内存空间、IO映射空间
    #if (OS_MMU_MEMORYMAP_USER_START + OS_MMU_MEMORYMAP_USER_SIZE) > OS_MMU_MEMORYMAP_KERNEL_START
        #error "User memory is overlapping with kernel memory!"
    #endif

    #if (OS_MMU_MEMORYMAP_KERNEL_START + OS_MMU_MEMORYMAP_KERNEL_SIZE) > OS_MMU_MEMORYMAP_IO_START
        #error "Kernel memory is overlapping with io memory!"??
    #endif

    //以下两个宏用于内核线性映射空间的PA/VA互转
    #define OS_MMU_PA_TO_VA(pa) ((pa) + OS_MMU_KERNEL_VA_PA_OFFSET)
    #define OS_MMU_VA_TO_PA(va) ((va) - OS_MMU_KERNEL_VA_PA_OFFSET)

    //MMU页表结构体
    typedef struct os_mmu_vtable
    {
        os_mmu_pt_l1_p l1_vtable;//L1页表指针
        //os_bitmap_t va_bitmap;
        os_size_t va_start;//虚拟空间起始地址
        os_size_t va_size;//虚拟空间大小
        //os_bool_t bitmap_initialized;
        os_bool_t allocated;//L1页表是否是自动分配的
        os_size_t refcnt;//引用计数
    }os_mmu_vtable_t,*os_mmu_vtable_p;

    os_err_t os_mmu_vtable_create(os_mmu_vtable_p vtable,os_mmu_pt_l1_p l1_vtable,os_size_t va_start,os_size_t va_size);
    //void os_mmu_vtable_bitmap_init(os_mmu_vtable_p vtable,void *memory);
    void os_mmu_vtable_remove(os_mmu_vtable_p vtable,os_bool_t remove_mapping);
    os_err_t os_mmu_create_mapping(os_mmu_vtable_p vtable,os_size_t va,os_size_t pa,os_size_t size,os_mmu_pt_prot_t prot);
    os_err_t os_mmu_remove_mapping(os_mmu_vtable_p vtable,os_size_t va,os_size_t size);
    os_err_t os_mmu_create_mapping_auto(os_mmu_vtable_p vtable,os_size_t va,os_size_t size,os_mmu_pt_prot_t prot);
    void *os_mmu_user_va_to_kernel_va(os_mmu_vtable_p vtable,os_size_t user_va);
    os_size_t os_mmu_find_vaddr(os_mmu_vtable_p vtable,os_size_t va_start,os_size_t size);
    void os_mmu_remove_all_mapping(os_mmu_vtable_p vtable);
    void os_mmu_switch(os_mmu_vtable_p vtable);
    os_mmu_vtable_p os_mmu_get_current_vtable();
    os_mmu_vtable_p os_mmu_get_kernel_pagetable();
    os_bool_t os_mmu_page_fault_handler(os_size_t addr,os_bool_t write);
    void os_mmu_preinit();
    void os_mmu_preinit_secondary();
    os_bool_t os_mmu_is_preinitialized();
    os_bool_t os_mmu_is_initialized();
    void os_mmu_init();
    void *os_mmu_create_io_mapping(os_mmu_vtable_p vtable,os_size_t pa,os_size_t size);
    void os_mmu_remove_io_mapping(os_mmu_vtable_p vtable,void *va,os_size_t size);
    os_bool_t os_mmu_io_mapping_copy(os_mmu_vtable_p vtable);
    os_err_t os_mmu_kernel_mapping_copy(os_mmu_vtable_p vtable);
    os_err_t os_mmu_user_mapping_copy(os_mmu_vtable_p dst_vtable,os_mmu_vtable_p src_vtable);

#endif