/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-06     lizhirui     the first version
 * 2021-07-08     lizhirui     add copy_from_user and execve syscall support
 */

// @formatter:off
#include <dreamos.h>

void arch_task_clone_stack_frame_init(struct TrapFrame *regs,os_task_t *task,os_size_t new_sp);

static os_err_t os_clone_copy_memory(os_task_p task)
{
    os_task_p cur_task = os_task_get_current_task();

    os_mmu_vtable_p src_vtable = cur_task -> vtable;
    os_mmu_vtable_p dst_vtable = task -> vtable;
    OS_ERR_GET_ERROR_AND_RETURN(os_mmu_vtable_create(dst_vtable,OS_NULL,OS_MMU_MEMORYMAP_USER_START,OS_MMU_MEMORYMAP_IO_START + OS_MMU_MEMORYMAP_IO_SIZE));
    os_mmu_io_mapping_copy(dst_vtable);
    os_mmu_kernel_mapping_copy(dst_vtable);
    OS_ERR_GET_ERROR_AND_RETURN(os_mmu_user_mapping_copy(dst_vtable,src_vtable));
    return OS_ERR_OK;
}

static os_err_t os_copy_from_user(void *kernel_mem,os_size_t user_mem,os_size_t mem_size)
{
    os_task_p task = os_task_get_current_task();

    OS_ERR_RETURN_ERROR(user_mem < OS_MMU_MEMORYMAP_USER_REAL_START,-OS_ERR_EINVAL);
    OS_ERR_RETURN_ERROR((user_mem) >= (OS_MMU_MEMORYMAP_USER_START + OS_MMU_MEMORYMAP_USER_SIZE),-OS_ERR_EINVAL);

    os_size_t cur_user_addr = user_mem;
    os_size_t cur_kernel_addr = (os_size_t)kernel_mem;
    os_size_t user_addr_uplimit = user_mem + mem_size;

    for(;cur_user_addr < user_addr_uplimit;)
    {
        os_size_t size = MIN(user_addr_uplimit - cur_user_addr,OS_MMU_PAGE_SIZE - OS_MMU_PAGE_OFFSET(cur_user_addr));
        void *user_kmem = os_mmu_user_va_to_kernel_va(task -> vtable,cur_user_addr);
        OS_ERR_RETURN_ERROR(user_kmem == OS_NULL,-OS_ERR_EINVAL);
        os_memcpy((void *)cur_kernel_addr,user_kmem,size);
        cur_user_addr += size;
        cur_kernel_addr += size;
    }

    return OS_ERR_OK;
}

os_ssize_t os_syscall_clone(struct TrapFrame *regs,os_size_t clone_flags,os_size_t newsp,os_size_t parent_tidptr,os_size_t child_tidptr,os_size_t tls)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    os_task_p cur_task = os_task_get_current_task();
    os_task_p task = os_memory_alloc(sizeof(os_task_t));
    OS_ERR_RETURN_ERROR(task == OS_NULL,-OS_ERR_EPERM);
    OS_ENTER_CRITICAL_AREA();
    os_err_t err;

    if((err = os_task_init(task,cur_task -> stack_size,cur_task -> priority,cur_task -> tick_init,cur_task -> entry,cur_task -> arg,cur_task -> name)) != OS_ERR_OK)
    {
        os_task_remove(task);
        OS_LEAVE_CRITICAL_AREA();
        return err;
    }

    os_list_node_remove(&task -> child_node);
    task -> parent = cur_task -> parent;
    os_list_insert_tail(task -> parent -> child_list,&task -> child_node);
    arch_task_clone_stack_frame_init(regs,task,newsp);

    if(clone_flags & OS_CLONE_VM)
    {
        task -> vtable = cur_task -> vtable;
        task -> vtable -> refcnt++;
    }
    else
    {
        task -> vtable -> refcnt--;
        task -> vtable = os_memory_alloc(sizeof(os_mmu_vtable_t));

        if(task -> vtable == OS_NULL)
        {
            os_task_remove(task);
            OS_LEAVE_CRITICAL_AREA();
            return err;
        }

        err = os_clone_copy_memory(task);

        if(err != OS_ERR_OK)
        {
            os_task_remove(task);
            OS_LEAVE_CRITICAL_AREA();
            return err;
        }
    }

    OS_LEAVE_CRITICAL_AREA();
    return OS_ERR_OK;
}

void arch_task_execve_stack_frame_init(struct TrapFrame *regs,os_size_t entry);

os_ssize_t os_syscall_execve(struct TrapFrame *regs,os_size_t filename,os_size_t argv,os_size_t argc)
{
    char *filename_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_RETURN_ERROR(filename_buf == OS_NULL,-OS_ERR_ENOMEM);
    os_err_t ret = OS_ERR_OK;
    OS_ERR_GET_ERROR_AND_GOTO(os_copy_from_user(filename_buf,filename,OS_VFS_PATH_MAX + 1),ret,err);
    filename_buf[OS_VFS_PATH_MAX] = '\0';
    os_size_t entry;
    OS_ERR_GET_ERROR_AND_GOTO(os_elf_load(filename_buf,&entry),ret,err);
    arch_task_execve_stack_frame_init(regs,entry);

err:
    os_memory_free(filename_buf);
    return ret;
}

os_ssize_t os_syscall_write()
{
    os_printf("syscall_write\n");
    return 0;
}

os_ssize_t os_syscall_exit()
{
    return 0;
}

os_ssize_t os_syscall_getpid()
{
    return (os_ssize_t)os_task_get_current_task() -> pid;
}

os_ssize_t os_syscall_getppid()
{
    os_task_p task = os_task_get_current_task();

    if(task -> parent)
    {
        return (os_ssize_t)task -> parent -> pid;
    }
    else
    {
        return -OS_ERR_EINVAL;
    }
}