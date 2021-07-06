/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __ARCH_MMU_H__
#define __ARCH_MMU_H__

    #define OS_MMU_PAGETABLE_LEVEL_NUM 3UL
    #define OS_MMU_MODE_STRING "Sv39"

    #define OS_MMU_L1_BITS 9UL
    #define OS_MMU_L2_BITS 9UL
    #define OS_MMU_L3_BITS 9UL
    #define OS_MMU_OFFSET_BITS 12UL

    #define OS_MMU_PHYSICAL_ADDR_LENGTH 56

    #define OS_MMU_MEMORYMAP_IO_START 0xFFFFFFC000000000UL
    #define OS_MMU_MEMORYMAP_IO_SIZE OS_GB(256)
    #define OS_MMU_MEMORYMAP_KERNEL_START 0x0000002000000000UL
    #define OS_MMU_MEMORYMAP_KERNEL_SIZE OS_GB(128)
    #define OS_MMU_MEMORYMAP_USER_START 0x0000000000001000UL
    #define OS_MMU_MEMORYMAP_USER_SIZE (OS_GB(128) - OS_MMU_MEMORYMAP_USER_START)

    #define OS_MMU_KERNEL_VA_PA_OFFSET (OS_MMU_MEMORYMAP_KERNEL_START - MEMORY_BASE)

    #ifndef __ASSEMBLY__
        #include <dreamos.h>

        typedef struct os_mmu_pt_l1
        {
            uint64_t value;
        }os_mmu_pt_l1_t;

        typedef struct os_mmu_pt_l2
        {
            uint64_t value;
        }os_mmu_pt_l2_t;

        typedef struct os_mmu_pt_l3
        {
            uint64_t value;
        }os_mmu_pt_l3_t;

        typedef struct os_mmu_pt_prot
        {
            uint64_t value;
        }os_mmu_pt_prot_t;

        #define OS_MMU_PROT(prot) ((os_mmu_pt_prot_t){(prot)})
        #define __MMU_PROT_VALUE(prot) ({os_mmu_pt_prot_t tmp = (prot);tmp.value;})

        #define __MMU_PROT_VALID (1 << 0)
        #define __MMU_PROT_READ (1 << 1)
        #define __MMU_PROT_WRITE (1 << 2)
        #define __MMU_PROT_EXECUTE (1 << 3)
        #define __MMU_PROT_USER (1 << 4)
        #define __MMU_PROT_GLOBAL (1 << 5)
        #define __MMU_PROT_ACCESSED (1 << 6)
        #define __MMU_PROT_DIRTY (1 << 7)

        #define OS_MMU_PROT_IO OS_MMU_PROT(__MMU_PROT_VALID | __MMU_PROT_READ | __MMU_PROT_WRITE | __MMU_PROT_GLOBAL)
        #define OS_MMU_PROT_KERNEL OS_MMU_PROT(__MMU_PROT_VALID | __MMU_PROT_READ | __MMU_PROT_WRITE | __MMU_PROT_EXECUTE | __MMU_PROT_GLOBAL)
        #define OS_MMU_PROT_USER OS_MMU_PROT(__MMU_PROT_VALID | __MMU_PROT_GLOBAL | __MMU_PROT_USER)
        #define OS_MMU_PROT_PAGETABLE OS_MMU_PROT(__MMU_PROT_VALID | __MMU_PROT_GLOBAL)

        #define OS_MMU_PROT_RO(prot) ({os_mmu_pt_prot_t *tmp = (prot);tmp -> value |= __MMU_PROT_READ;tmp -> value &= ~(__MMU_PROT_WRITE | __MMU_PROT_EXECUTE);})
        #define OS_MMU_PROT_RW(prot) ({os_mmu_pt_prot_t *tmp = (prot);tmp -> value |= (__MMU_PROT_READ | __MMU_PROT_WRITE);tmp -> value &= ~__MMU_PROT_EXECUTE;})
        #define OS_MMU_PROT_RX(prot) ({os_mmu_pt_prot_t *tmp = (prot);tmp -> value |= (__MMU_PROT_READ | __MMU_PROT_EXECUTE);tmp -> value &= ~__MMU_PROT_WRITE;})
        #define OS_MMU_PROT_RWX(prot) ({os_mmu_pt_prot_t *tmp = (prot);tmp -> value |= (__MMU_PROT_READ | __MMU_PROT_WRITE | __MMU_PROT_EXECUTE);})

        #define __MMU_ENTRY_PPN_OFFSET_SHIFT 10
        
        #define OS_MMU_L1_ENTRY(pa,prot) ((os_mmu_pt_l1_t){((OS_MMU_PA_TO_PPN(pa) << __MMU_ENTRY_PPN_OFFSET_SHIFT) | __MMU_PROT_VALUE(prot))})
        #define OS_MMU_L2_ENTRY(pa,prot) ((os_mmu_pt_l2_t){((OS_MMU_PA_TO_PPN(pa) << __MMU_ENTRY_PPN_OFFSET_SHIFT) | __MMU_PROT_VALUE(prot))})
        #define OS_MMU_L3_ENTRY(pa,prot) ((os_mmu_pt_l3_t){((OS_MMU_PA_TO_PPN(pa) << __MMU_ENTRY_PPN_OFFSET_SHIFT) | __MMU_PROT_VALUE(prot))})

        #define OS_MMU_L1_NULL_ENTRY ((os_mmu_pt_l1_t){(0)})
        #define OS_MMU_L2_NULL_ENTRY ((os_mmu_pt_l2_t){(0)})
        #define OS_MMU_L3_NULL_ENTRY ((os_mmu_pt_l3_t){(0)})

        #define OS_MMU_FLUSH_TLB() do{asm volatile("sfence.vma");}while(0)

        #define OS_MMU_GET_EFFECTIVE_VA(va) MASK_VALUE(va,MASK(OS_MMU_L1_BITS + OS_MMU_L2_BITS + OS_MMU_L3_BITS + OS_MMU_OFFSET_BITS))
        #define OS_MMU_GET_REAL_VA(va) (OS_MMU_GET_EFFECTIVE_VA(va) | (((va >> 38) & 0x01) ? 0xFFFFFF8000000000UL : 0UL))
    #endif
#endif