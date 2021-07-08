/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-04     lizhirui     the first version
 * 2021-07-05     lizhirui     add vaddr find support
 */

// @formatter:off
#include <dreamos.h>

static os_bool_t os_mmu_preinitialized = OS_FALSE;

static inline os_bool_t __is_null_entry(os_size_t entry)
{
    return entry == 0;
}

static inline os_size_t __get_ppn(os_size_t entry)
{
    return entry >> __MMU_ENTRY_PPN_OFFSET_SHIFT;
}

static inline os_bool_t __is_pagetable(os_size_t entry)
{
    return ((entry & (__MMU_PROT_WRITE | __MMU_PROT_READ | __MMU_PROT_EXECUTE)) == 0) && (__get_ppn(entry) != 0);
}

static os_err_t __create_l3_entry(os_mmu_pt_l3_t *vtable,os_size_t va,os_size_t pa,os_size_t size,os_mmu_pt_prot_t prot)
{
    os_size_t l3_id = OS_MMU_L3_ID(va);
    os_size_t ppn = OS_MMU_PA_TO_PPN(pa);
    os_size_t va_end = va + size - 1;
    os_size_t l3_id_end = OS_MMU_L3_ID(va_end);
    
    for(;l3_id <= l3_id_end;l3_id++,ppn++)
    {
        vtable[l3_id] = OS_MMU_L3_ENTRY(OS_MMU_PPN_TO_PA(ppn),prot);
    }

    return OS_ERR_OK;
}

static os_err_t __create_l2_entry(os_mmu_pt_l2_t *vtable,os_size_t va,os_size_t pa,os_size_t size,os_mmu_pt_prot_t prot)
{
    os_size_t l2_id = OS_MMU_L2_ID(va);
    os_size_t va_end = va + size - 1;
    os_size_t l2_id_end = OS_MMU_L2_ID(va_end);

    for(;l2_id <= l2_id_end;l2_id++)
    {
        if(OS_MMU_L2_ALIGN(va) && (size >= OS_MMU_L2_SIZE) && (OS_MMU_L2_CHECK_ALIGN(pa)))
        {
            vtable[l2_id] = OS_MMU_L2_ENTRY(pa,prot);
            va += OS_MMU_L2_SIZE;
            pa += OS_MMU_L2_SIZE;
            size -= OS_MMU_L2_SIZE;
            continue;
        }

        if(!__is_pagetable(vtable[l2_id].value))
        {
            if(__is_null_entry(vtable[l2_id].value))
            {
                OS_ANNOTATION_NEED_MMU_PREINIT();
                os_size_t l3_vtable = (os_size_t)os_memory_alloc(OS_MMU_L3_PAGES * OS_MMU_PAGE_SIZE);
                os_memset((void *)l3_vtable,0,OS_MMU_L3_PAGES * OS_MMU_PAGE_SIZE);

                if(!l3_vtable)
                {
                    return -OS_ERR_ENOMEM;
                }
                
                vtable[l2_id] = OS_MMU_L2_ENTRY(OS_MMU_VA_TO_PA(l3_vtable),OS_MMU_PROT_PAGETABLE);
            }
            else
            {
                return -OS_ERR_EPERM;
            }
        }

        os_mmu_pt_l3_t *next_vtable = (os_mmu_pt_l3_t *)(os_mmu_is_preinitialized() ? OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable[l2_id].value))) : OS_MMU_PPN_TO_PA(__get_ppn(vtable[l2_id].value)));
        os_size_t group_va_end = (OS_MMU_L2_ID(va) == OS_MMU_L2_ID(va_end)) ? va_end : (va | (OS_MMU_L2_SIZE - 1));
        os_size_t next_size = group_va_end - va + 1;
        os_err_t err = __create_l3_entry(next_vtable,va,pa,next_size,prot);

        if(err != OS_ERR_OK)
        {
            return err;
        }

        va += next_size;
        pa += next_size;
        size -= next_size;
    }

    return OS_ERR_OK;
}

os_err_t os_mmu_create_mapping(os_mmu_vtable_p vtable,os_size_t va,os_size_t pa,os_size_t size,os_mmu_pt_prot_t prot)
{
    os_size_t l1_id = OS_MMU_L1_ID(va);
    os_size_t va_end = va + size - 1;
    os_size_t l1_id_end = OS_MMU_L1_ID(va_end);

    va = OS_MMU_GET_EFFECTIVE_VA(va);

    OS_ASSERT(va >= vtable -> va_start);
    OS_ASSERT((va + size) <= (vtable -> va_start + vtable -> va_size));

    for(;l1_id <= l1_id_end;l1_id++)
    {
        if(OS_MMU_L1_ALIGN(va) && (size >= OS_MMU_L1_SIZE) && (OS_MMU_L1_CHECK_ALIGN(pa)) && (va < OS_MMU_MEMORYMAP_IO_START))
        {
            vtable -> l1_vtable[l1_id] = OS_MMU_L1_ENTRY(pa,prot);
            va += OS_MMU_L1_SIZE;
            pa += OS_MMU_L1_SIZE;
            size -= OS_MMU_L1_SIZE;
            continue;
        }

        if(!__is_pagetable(vtable -> l1_vtable[l1_id].value))
        {
            if(__is_null_entry(vtable -> l1_vtable[l1_id].value))
            {
                os_size_t l2_vtable = (os_size_t)os_memory_alloc(OS_MMU_L2_PAGES * OS_MMU_PAGE_SIZE);
                os_memset((void *)l2_vtable,0,OS_MMU_L2_PAGES * OS_MMU_PAGE_SIZE);

                if(!l2_vtable)
                {
                    return -OS_ERR_ENOMEM;
                }
                
                vtable -> l1_vtable[l1_id] = OS_MMU_L1_ENTRY(OS_MMU_VA_TO_PA(l2_vtable),OS_MMU_PROT_PAGETABLE);
            }
            else
            {
                return -OS_ERR_EPERM;
            }
        }

        os_mmu_pt_l2_t *next_vtable = (os_mmu_pt_l2_t *)(os_mmu_is_preinitialized() ? OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable -> l1_vtable[l1_id].value))) : OS_MMU_PPN_TO_PA(__get_ppn(vtable -> l1_vtable[l1_id].value)));
        os_size_t group_va_end = (OS_MMU_L1_ID(va) == OS_MMU_L1_ID(va_end)) ? va_end : (va | (OS_MMU_L1_SIZE - 1));
        os_size_t next_size = group_va_end - va + 1;
        os_err_t err = __create_l2_entry(next_vtable,va,pa,next_size,prot);

        if(err != OS_ERR_OK)
        {
            return err;
        }

        va += next_size;
        pa += next_size;
        size -= next_size;
    }

    return OS_ERR_OK;
}

static os_err_t __remove_l3_entry(os_mmu_pt_l3_t *vtable,os_size_t va,os_size_t size)
{
    os_size_t l3_id = OS_MMU_L3_ID(va);
    os_size_t va_end = va + size - 1;
    os_size_t l3_id_end = OS_MMU_L3_ID(va_end);
    
    for(;l3_id <= l3_id_end;l3_id++)
    {
        vtable[l3_id] = OS_MMU_L3_NULL_ENTRY;
    }

    return OS_ERR_OK;
}

static os_err_t __remove_l2_entry(os_mmu_pt_l2_t *vtable,os_size_t va,os_size_t size)
{
    os_size_t l2_id = OS_MMU_L2_ID(va);
    os_size_t va_end = va + size - 1;
    os_size_t l2_id_end = OS_MMU_L2_ID(va_end);

    for(;l2_id <= l2_id_end;l2_id++)
    {
        if(OS_MMU_L2_ALIGN(va) && (size >= OS_MMU_L2_SIZE) && (!__is_pagetable(vtable[l2_id].value)))
        {
            vtable[l2_id] = OS_MMU_L2_NULL_ENTRY;
            va += OS_MMU_L2_SIZE;
            size -= OS_MMU_L2_SIZE;
            continue;
        }

        os_size_t group_va_end = (OS_MMU_L2_ID(va) == OS_MMU_L2_ID(va_end)) ? va_end : (va | (OS_MMU_L2_SIZE - 1));
        os_size_t next_size = group_va_end - va + 1;

        if(!__is_pagetable(vtable[l2_id].value))
        {
            if(__is_null_entry(vtable[l2_id].value))
            {
                va += next_size;
                size -= next_size;
                continue;
            }
            else
            {
                return -OS_ERR_EPERM;
            }
        }

        os_mmu_pt_l3_t *next_vtable = (os_mmu_pt_l3_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable[l2_id].value)));
        os_err_t err = __remove_l3_entry(next_vtable,va,next_size);

        if(err != OS_ERR_OK)
        {
            return err;
        }

        va += next_size;
        size -= next_size;
    }

    return OS_ERR_OK;
}

os_err_t os_mmu_remove_mapping(os_mmu_vtable_p vtable,os_size_t va,os_size_t size)
{
    os_size_t l1_id = OS_MMU_L1_ID(va);
    os_size_t va_end = va + size - 1;
    os_size_t l1_id_end = OS_MMU_L1_ID(va_end);

    for(;l1_id <= l1_id_end;l1_id++)
    {
        if(OS_MMU_L1_ALIGN(va) && (size >= OS_MMU_L1_SIZE) && (!__is_pagetable(vtable -> l1_vtable[l1_id].value)))
        {
            vtable -> l1_vtable[l1_id] = OS_MMU_L1_NULL_ENTRY;
            va += OS_MMU_L1_SIZE;
            size -= OS_MMU_L1_SIZE;
            continue;
        }

        os_size_t group_va_end = (OS_MMU_L1_ID(va) == OS_MMU_L1_ID(va_end)) ? va_end : (va | (OS_MMU_L1_SIZE - 1));
        os_size_t next_size = group_va_end - va + 1;

        if(!__is_pagetable(vtable -> l1_vtable[l1_id].value))
        {
            if(__is_null_entry(vtable -> l1_vtable[l1_id].value))
            {
                va += next_size;
                size -= next_size;
                continue;
            }
            else
            {
                return -OS_ERR_EPERM;
            }
        }

        os_mmu_pt_l2_t *next_vtable = (os_mmu_pt_l2_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable -> l1_vtable[l1_id].value)));
        os_err_t err = __remove_l2_entry(next_vtable,va,next_size);

        if(err != OS_ERR_OK)
        {
            return err;
        }

        va += next_size;
        size -= next_size;
    }

    return OS_ERR_OK;
}

static void os_mmu_remove_all_mapping_l3(os_mmu_pt_l3_t *vtable,os_bool_t is_user_page)
{
    os_size_t i;

    for(i = 0;i < OS_MMU_L3_ENTRY_NUM;i++)
    {
        if(!__is_null_entry(vtable[i].value))
        {
            OS_ASSERT(!__is_pagetable(vtable[i].value));

            if(is_user_page)
            {
                os_memory_free((void *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable[i].value))));
            }
        }
    }
}

static void os_mmu_remove_all_mapping_l2(os_mmu_pt_l2_t *vtable,os_bool_t is_user_page)
{
    os_size_t i;

    for(i = 0;i < OS_MMU_L2_ENTRY_NUM;i++)
    {
        if(!__is_null_entry(vtable[i].value))
        {
            if(__is_pagetable(vtable[i].value))
            {
                os_mmu_pt_l3_t *next_vtable = (os_mmu_pt_l3_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable[i].value)));
                os_mmu_remove_all_mapping_l3(next_vtable,is_user_page);
                os_memory_free((void *)OS_MMU_VA_TO_PA((os_size_t)next_vtable));
            }
            else if(is_user_page)
            {
                os_memory_free((void *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable[i].value))));
            }
        }
    }
}

void os_mmu_remove_all_mapping(os_mmu_vtable_p vtable)
{
    os_size_t i;
    os_size_t user_l1_start = OS_MMU_L1_ID(OS_MMU_MEMORYMAP_USER_START);
    os_size_t user_l1_end = OS_MMU_L1_ID(OS_MMU_MEMORYMAP_USER_START + OS_MMU_MEMORYMAP_USER_SIZE - 1);

    for(i = 0;i < OS_MMU_L1_ENTRY_NUM;i++)
    {
        if(!__is_null_entry(vtable -> l1_vtable[i].value))
        {
            if(__is_pagetable(vtable -> l1_vtable[i].value))
            {
                os_mmu_pt_l2_t *next_vtable = (os_mmu_pt_l2_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable -> l1_vtable[i].value)));
                os_mmu_remove_all_mapping_l2(next_vtable,(i >= user_l1_start) && (i <= user_l1_end));
                os_memory_free((void *)OS_MMU_VA_TO_PA((os_size_t)next_vtable));
            }
            else if((i >= user_l1_start) && (i <= user_l1_end))
            {
                os_memory_free((void *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable -> l1_vtable[i].value))));
            }
        }
    }
}

os_size_t os_mmu_find_vaddr(os_mmu_vtable_p vtable,os_size_t va_start,os_size_t size)
{
    os_size_t va_ret = va_start;
    os_size_t va_end = va_start + size - 1;
    os_size_t remainedsize = size;
    os_size_t va;

    for(va = va_start;va <= va_end;)
    {
        os_size_t l1_id = OS_MMU_L1_ID(va);
        os_size_t l2_id = OS_MMU_L2_ID(va);
        os_size_t l3_id = OS_MMU_L3_ID(va);

        if(__is_null_entry(vtable -> l1_vtable[l1_id].value))
        {
            va += MIN(remainedsize,OS_MMU_L1_SIZE - OS_MMU_L1_OFFSET(va));
            remainedsize -= MIN(remainedsize,OS_MMU_L1_SIZE - OS_MMU_L1_OFFSET(va));
        }
        else if(!__is_pagetable(vtable -> l1_vtable[l1_id].value))
        {
            va_ret = va + OS_MMU_L1_SIZE - OS_MMU_L1_OFFSET(va);
            remainedsize = size;
            va += OS_MMU_L1_SIZE - OS_MMU_L1_OFFSET(va);
        }
        else
        {
            os_mmu_pt_l2_p l2_vtable = (os_mmu_pt_l2_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable -> l1_vtable[l1_id].value)));

            if(__is_null_entry(l2_vtable[l2_id].value))
            {
                va += MIN(remainedsize,OS_MMU_L2_SIZE - OS_MMU_L2_OFFSET(va));
                remainedsize -= MIN(remainedsize,OS_MMU_L2_SIZE - OS_MMU_L2_OFFSET(va));
            }
            else if(!__is_pagetable(l2_vtable[l2_id].value))
            {
                va_ret = va + OS_MMU_L2_SIZE - OS_MMU_L2_OFFSET(va);
                remainedsize = size;
                va += OS_MMU_L2_SIZE - OS_MMU_L2_OFFSET(va);
            }
            else
            {
                os_mmu_pt_l3_p l3_vtable = (os_mmu_pt_l3_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(l2_vtable[l2_id].value)));

                if(__is_null_entry(l3_vtable[l3_id].value))
                {
                    va += MIN(remainedsize,OS_MMU_L3_SIZE - OS_MMU_L3_OFFSET(va));
                    remainedsize -= MIN(remainedsize,OS_MMU_L3_SIZE - OS_MMU_L3_OFFSET(va));
                }
                else
                {
                    OS_ASSERT(!__is_pagetable(l3_vtable[l3_id].value));
                    va_ret = va + OS_MMU_L3_SIZE - OS_MMU_L3_OFFSET(va);
                    remainedsize = size;
                    va += OS_MMU_L3_SIZE - OS_MMU_L3_OFFSET(va);
                }
            }
        }

        if(remainedsize == 0)
        {
            return va_ret;
        }
    }

    return 0;
}

void arch_mmu_switch(os_mmu_vtable_p vtable)
{
    os_size_t vtable_addr = os_mmu_is_preinitialized() ? OS_MMU_VA_TO_PA((os_size_t)vtable -> l1_vtable) : ((os_size_t)vtable -> l1_vtable);
    SYNC_DATA();
    SYNC_INSTRUCTION();
    write_csr(satp,(vtable_addr >> OS_MMU_OFFSET_BITS) | (((os_size_t)SATP_MODE_SV39) << 60));
    OS_MMU_FLUSH_TLB();
}

os_mmu_pt_l1_t kernel_l1_pagetable[OS_MMU_L1_ENTRY_NUM] __aligned(OS_MMU_PAGE_SIZE);
os_mmu_vtable_t kernel_pagetable;
os_mmu_pt_l1_t kernel_l1_jump_pagetable[OS_MMU_L1_ENTRY_NUM] __aligned(OS_MMU_PAGE_SIZE);
os_mmu_vtable_t kernel_jump_pagetable;
void __enable_mmu();

os_bool_t os_mmu_io_mapping_copy(os_mmu_vtable_p vtable)
{
    os_size_t l1_id_start = OS_MMU_L1_ID(OS_MMU_MEMORYMAP_IO_START);
    os_size_t l1_id_end = OS_MMU_L1_ID(OS_MMU_MEMORYMAP_IO_START + OS_MMU_MEMORYMAP_IO_SIZE - 1);

    if(os_memcmp(&vtable -> l1_vtable[l1_id_start],&kernel_pagetable.l1_vtable[l1_id_start],(l1_id_end - l1_id_start + 1) * sizeof(os_mmu_pt_l1_t)) == 0)
    {
        return OS_FALSE;
    }

    os_memcpy(&vtable -> l1_vtable[l1_id_start],&kernel_pagetable.l1_vtable[l1_id_start],(l1_id_end - l1_id_start + 1) * sizeof(os_mmu_pt_l1_t));
    return OS_TRUE;
}

os_err_t os_mmu_kernel_mapping_copy(os_mmu_vtable_p vtable)
{
    os_size_t l1_id_start = OS_MMU_L1_ID(OS_MMU_MEMORYMAP_KERNEL_START);
    os_size_t l1_id_end = OS_MMU_L1_ID(OS_MMU_MEMORYMAP_KERNEL_START + OS_MMU_MEMORYMAP_KERNEL_SIZE - 1);

    if(os_memcmp(&vtable -> l1_vtable[l1_id_start],&kernel_pagetable.l1_vtable[l1_id_start],(l1_id_end - l1_id_start + 1) * sizeof(os_mmu_pt_l1_t)) == 0)
    {
        return -OS_ERR_EPERM;
    }

    os_memcpy(&vtable -> l1_vtable[l1_id_start],&kernel_pagetable.l1_vtable[l1_id_start],(l1_id_end - l1_id_start + 1) * sizeof(os_mmu_pt_l1_t));
    return OS_ERR_OK;
}

static os_err_t os_mmu_user_mapping_copy_l3(os_mmu_pt_l3_p dst_vtable,os_mmu_pt_l3_p src_vtable)
{
    os_size_t i;
    os_err_t ret;

    for(i = 0;i < OS_MMU_L3_ENTRY_NUM;i++)
    {
        OS_ASSERT(!__is_pagetable(dst_vtable[i].value));

        if(!__is_null_entry(dst_vtable[i].value))
        {
            void *dst_mem = os_memory_alloc(OS_MMU_L3_SIZE);
            OS_ERR_SET_ERROR_AND_GOTO(dst_mem == OS_NULL,ret,-OS_ERR_ENOMEM,err);
            dst_vtable[i] = OS_MMU_L3_ENTRY(OS_MMU_VA_TO_PA((os_size_t)dst_mem),OS_MMU_PROT(__MMU_GET_PROT(src_vtable[i].value)));
            void *src_mem = (void *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(src_vtable[i].value)));
            os_memcpy(dst_mem,src_mem,OS_MMU_L3_SIZE);
        }
    }

    return OS_ERR_OK;

err:
    return ret;
}

static os_err_t os_mmu_user_mapping_copy_l2(os_mmu_pt_l2_p dst_vtable,os_mmu_pt_l2_p src_vtable)
{
    os_size_t i;
    os_err_t ret;

    for(i = 0;i < OS_MMU_L2_ENTRY_NUM;i++)
    {
        if(__is_pagetable(dst_vtable[i].value))
        {
            os_mmu_pt_l3_p dst_next_vtable = os_memory_alloc(sizeof(os_mmu_pt_l3_t));
            OS_ERR_SET_ERROR_AND_GOTO(dst_next_vtable == OS_NULL,ret,-OS_ERR_ENOMEM,err);
            dst_vtable[i] = OS_MMU_L2_ENTRY(OS_MMU_VA_TO_PA((os_size_t)dst_next_vtable),OS_MMU_PROT(__MMU_GET_PROT(src_vtable[i].value)));
            os_mmu_pt_l3_p src_next_vtable = (os_mmu_pt_l3_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(src_vtable[i].value)));
            os_memcpy(dst_next_vtable,src_next_vtable,sizeof(os_mmu_pt_l3_t));
            OS_ERR_GET_ERROR_AND_GOTO(os_mmu_user_mapping_copy_l3(dst_next_vtable,src_next_vtable),ret,err);
        }
        else if(!__is_null_entry(dst_vtable[i].value))
        {
            void *dst_mem = os_memory_alloc(OS_MMU_L2_SIZE);
            OS_ERR_SET_ERROR_AND_GOTO(dst_mem == OS_NULL,ret,-OS_ERR_ENOMEM,err);
            dst_vtable[i] = OS_MMU_L2_ENTRY(OS_MMU_VA_TO_PA((os_size_t)dst_mem),OS_MMU_PROT(__MMU_GET_PROT(src_vtable[i].value)));
            void *src_mem = (void *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(src_vtable[i].value)));
            os_memcpy(dst_mem,src_mem,OS_MMU_L2_SIZE);
        }
    }

    return OS_ERR_OK;

err:
    return ret;
}

os_err_t os_mmu_user_mapping_copy(os_mmu_vtable_p dst_vtable,os_mmu_vtable_p src_vtable)
{
    os_size_t l1_id_start = OS_MMU_L1_ID(OS_MMU_MEMORYMAP_USER_START);
    os_size_t l1_id_end = OS_MMU_L1_ID(OS_MMU_MEMORYMAP_USER_START + OS_MMU_MEMORYMAP_USER_SIZE - 1);

    os_memcpy(&dst_vtable -> l1_vtable[l1_id_start],&kernel_pagetable.l1_vtable[l1_id_start],(l1_id_end - l1_id_start + 1) * sizeof(os_mmu_pt_l1_t));

    os_size_t i;
    os_err_t ret;

    for(i = l1_id_start;i <= l1_id_end;i++)
    {
        if(__is_pagetable(dst_vtable -> l1_vtable[i].value))
        {
            os_mmu_pt_l2_p dst_next_vtable = os_memory_alloc(sizeof(os_mmu_pt_l2_t));
            OS_ERR_SET_ERROR_AND_GOTO(dst_next_vtable == OS_NULL,ret,-OS_ERR_ENOMEM,err);
            dst_vtable -> l1_vtable[i] = OS_MMU_L1_ENTRY(OS_MMU_VA_TO_PA((os_size_t)dst_next_vtable),OS_MMU_PROT(__MMU_GET_PROT(src_vtable -> l1_vtable[i].value)));
            os_mmu_pt_l2_p src_next_vtable = (os_mmu_pt_l2_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(src_vtable -> l1_vtable[i].value)));
            os_memcpy(dst_next_vtable,src_next_vtable,sizeof(os_mmu_pt_l2_t));
            OS_ERR_GET_ERROR_AND_GOTO(os_mmu_user_mapping_copy_l2(dst_next_vtable,src_next_vtable),ret,err);
        }
        else if(!__is_null_entry(dst_vtable -> l1_vtable[i].value))
        {
            void *dst_mem = os_memory_alloc(OS_MMU_L1_SIZE);
            OS_ERR_SET_ERROR_AND_GOTO(dst_mem == OS_NULL,ret,-OS_ERR_ENOMEM,err);
            dst_vtable -> l1_vtable[i] = OS_MMU_L1_ENTRY(OS_MMU_VA_TO_PA((os_size_t)dst_mem),OS_MMU_PROT(__MMU_GET_PROT(src_vtable -> l1_vtable[i].value)));
            void *src_mem = (void *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(src_vtable -> l1_vtable[i].value)));
            os_memcpy(dst_mem,src_mem,OS_MMU_L1_SIZE);
        }
    }

    return OS_ERR_OK;

err:
    os_mmu_vtable_remove(dst_vtable,OS_TRUE);
    return ret;
}

void *os_mmu_user_va_to_kernel_va(os_mmu_vtable_p vtable,os_size_t user_va)
{
    os_size_t l1_id = OS_MMU_L1_ID(user_va);
    os_size_t l2_id = OS_MMU_L2_ID(user_va);
    os_size_t l3_id = OS_MMU_L3_ID(user_va);
    os_size_t offset = OS_MMU_PAGE_OFFSET(user_va);

    if(__is_null_entry(vtable -> l1_vtable[l1_id].value))
    {
        return OS_NULL;
    }
    else if(!__is_pagetable(vtable -> l1_vtable[l1_id].value))
    {
        return OS_NULL;
    }
    else
    {
        os_mmu_pt_l2_p l2_vtable = (os_mmu_pt_l2_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(vtable -> l1_vtable[l1_id].value)));

        if(__is_null_entry(l2_vtable[l2_id].value))
        {
            return OS_NULL;
        }
        else if(!__is_pagetable(l2_vtable[l2_id].value))
        {
            return OS_NULL;
        }
        else
        {
            os_mmu_pt_l3_p l3_vtable = (os_mmu_pt_l3_t *)OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(l2_vtable[l2_id].value)));

            if(__is_null_entry(l3_vtable[l3_id].value))
            {
                return OS_NULL;
            }
            else
            {
                OS_ASSERT(!__is_pagetable(l3_vtable[l3_id].value));
                return (void *)(OS_MMU_PA_TO_VA(OS_MMU_PPN_TO_PA(__get_ppn(l3_vtable[l3_id].value))) + offset);
            }
        }
    }

    return OS_NULL;
}

void os_mmu_preinit()
{
    os_mmu_preinitialized = OS_FALSE;
    
    OS_ASSERT(os_mmu_vtable_create(&kernel_pagetable,(os_mmu_pt_l1_p)&kernel_l1_pagetable,OS_MMU_MEMORYMAP_KERNEL_START,OS_MMU_MEMORYMAP_KERNEL_SIZE + OS_MMU_MEMORYMAP_IO_SIZE) == OS_ERR_OK);
    OS_ASSERT(os_mmu_create_mapping(&kernel_pagetable,OS_MMU_MEMORYMAP_KERNEL_START,MEMORY_BASE,DIV_UP(MEMORY_SIZE,OS_MMU_L1_SIZE) * OS_MMU_L1_SIZE,OS_MMU_PROT_KERNEL) == OS_ERR_OK);

    OS_ASSERT(os_mmu_vtable_create(&kernel_jump_pagetable,(os_mmu_pt_l1_p)&kernel_l1_jump_pagetable,OS_MMU_MEMORYMAP_USER_START,OS_MMU_MEMORYMAP_USER_SIZE + OS_MMU_MEMORYMAP_KERNEL_SIZE) == OS_ERR_OK);
    OS_ASSERT(os_mmu_create_mapping(&kernel_jump_pagetable,MEMORY_BASE,MEMORY_BASE,DIV_UP(MEMORY_SIZE,OS_MMU_L1_SIZE) * OS_MMU_L1_SIZE,OS_MMU_PROT_KERNEL) == OS_ERR_OK);
    OS_ASSERT(os_mmu_create_mapping(&kernel_jump_pagetable,OS_MMU_MEMORYMAP_KERNEL_START,MEMORY_BASE,DIV_UP(MEMORY_SIZE,OS_MMU_L1_SIZE) * OS_MMU_L1_SIZE,OS_MMU_PROT_KERNEL) == OS_ERR_OK);
}

void os_mmu_preinit_secondary()
{
    os_mmu_switch(&kernel_pagetable);
    kernel_pagetable.l1_vtable = (os_mmu_pt_l1_p)OS_MMU_PA_TO_VA((os_size_t)kernel_pagetable.l1_vtable);
    os_mmu_vtable_remove(&kernel_jump_pagetable,OS_FALSE);
    os_mmu_preinitialized = OS_TRUE;
}

os_bool_t os_mmu_is_preinitialized()
{
    return os_mmu_preinitialized;
}

os_mmu_vtable_p os_mmu_get_kernel_pagetable()
{
    return (os_mmu_vtable_p)&kernel_pagetable;
}