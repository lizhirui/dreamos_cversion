/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __OS_MMU_H__
#define __OS_MMU_H__

    #include <dreamos.h>
    #include <arch.h>

    #if (OS_MMU_PAGETABLE_LEVEL_NUM > 4UL) || (OS_MMU_PAGETABLE_LEVEL_NUM < 1UL)
        #error "DreamOS only support 1~4 level pagetable!"
    #endif

    #define OS_MMU_PAGE_SIZE SIZE(OS_MMU_OFFSET_BITS)

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

    #define OS_MMU_L1_SHIFT (OS_MMU_L2_SHIFT + OS_MMU_L2_BITS)
    #define OS_MMU_L1_SIZE SIZE(OS_MMU_L1_SHIFT)
    #define OS_MMU_L1_ENTRY_NUM SIZE(OS_MMU_L1_BITS)
    #define OS_MMU_L1_MASK (MASK(OS_MMU_L1_BITS) << OS_MMU_L1_SHIFT)
    #define OS_MMU_L1_ID(va) (((va) & OS_MMU_L1_MASK) >> OS_MMU_L1_SHIFT)
    #define OS_MMU_L1_PAGES DIV_UP(OS_MMU_L1_ENTRY_NUM * sizeof(os_mmu_pt_l1_t),OS_MMU_PAGE_SIZE)
    #define OS_MMU_L1_CHECK_ALIGN(va) (((va) & MASK(OS_MMU_L1_SHIFT)) == 0)
    #define OS_MMU_L1_ALIGN(va) (((va) & UMASK(OS_MMU_L1_SHIFT)))
    #define OS_MMU_L1_OFFSET(va) (((va) & MASK(OS_MMU_L1_SHIFT)))
    typedef os_mmu_pt_l1_t *os_mmu_pt_l1_p;

    #define OS_MMU_PA_TO_PPN(pa) ((pa) >> OS_MMU_OFFSET_BITS)
    #define OS_MMU_PPN_TO_PA(ppn) ((ppn) << OS_MMU_OFFSET_BITS)

    #if (OS_MMU_MEMORYMAP_USER_START + OS_MMU_MEMORYMAP_USER_SIZE) > OS_MMU_MEMORYMAP_KERNEL_START
        #error "User memory is overlapping with kernel memory!"
    #endif

    #if (OS_MMU_MEMORYMAP_KERNEL_START + OS_MMU_MEMORYMAP_KERNEL_SIZE) > OS_MMU_MEMORYMAP_IO_START
        #error "Kernel memory is overlapping with io memory!"
    #endif

    #define OS_MMU_PA_TO_VA(pa) ((pa) + OS_MMU_KERNEL_VA_PA_OFFSET)
    #define OS_MMU_VA_TO_PA(va) ((va) - OS_MMU_KERNEL_VA_PA_OFFSET)

    typedef struct os_mmu_vtable
    {
        os_mmu_pt_l1_p l1_vtable;
        //os_bitmap_t va_bitmap;
        os_size_t va_start;
        os_size_t va_size;
        //os_bool_t bitmap_initialized;
        os_bool_t allocated;
        os_size_t refcnt;
    }os_mmu_vtable_t,*os_mmu_vtable_p;
    
    void os_mmu_vtable_create(os_mmu_vtable_p vtable,os_mmu_pt_l1_p l1_vtable,os_size_t va_start,os_size_t va_size);
    //void os_mmu_vtable_bitmap_init(os_mmu_vtable_p vtable,void *memory);
    void os_mmu_vtable_remove(os_mmu_vtable_p vtable);
    os_err_t os_mmu_create_mapping(os_mmu_vtable_p vtable,os_size_t va,os_size_t pa,os_size_t size,os_mmu_pt_prot_t prot);
    os_err_t os_mmu_remove_mapping(os_mmu_vtable_p vtable,os_size_t va,os_size_t size);
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

#endif