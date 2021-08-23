/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-06     lizhirui     the first version
 * 2021-07-08     lizhirui     add copy_from_user and execve syscall support
 * 2021-07-09     lizhirui     add some syscalls
 */

// @formatter:off
#include <dreamos.h>

/*!
 * 用于实现将内存从用户态拷贝到内核态，通常用于系统调用入口从用户内存读取数据时
 * @param kernel_mem 内核态缓冲区
 * @param user_mem 用户态地址
 * @param mem_size 要拷贝的数据量（字节）
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
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

/*!
 * 用于实现将内存从内核态拷贝到用户态，通常用于系统调用出口将数据写回用户内存时
 * @param user_mem 用户态地址
 * @param kernel_mem 内核态缓冲区
 * @param mem_size 要拷贝的数据量（字节）
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
static os_err_t os_copy_to_user(os_size_t user_mem,void *kernel_mem,os_size_t mem_size)
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
        os_memcpy(user_kmem,(void *)cur_kernel_addr,size);
        cur_user_addr += size;
        cur_kernel_addr += size;
    }

    return OS_ERR_OK;
}

os_ssize_t os_syscall_getcwd(struct TrapFrame *regs,os_size_t buf,os_size_t size)
{
    return 0;
}

os_ssize_t os_syscall_pipe2(struct TrapFrame *regs,os_size_t *fd)
{
    return 0;
}

os_ssize_t os_syscall_dup(struct TrapFrame *regs,os_size_t fd)
{
    return 0;
}

os_ssize_t os_syscall_dup3(struct TrapFrame *regs,os_size_t old,os_size_t new)
{
    return 0;
}

os_ssize_t os_syscall_chdir(struct TrapFrame *regs,os_size_t path)
{
    return 0;
}

os_ssize_t os_syscall_openat(struct TrapFrame *regs,os_size_t fd,os_size_t filename,os_size_t flags,os_size_t mode)
{
    char *filename_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_RETURN_ERROR(filename_buf == OS_NULL,-OS_ERR_ENOMEM);
    os_err_t ret = OS_ERR_OK;
    OS_ERR_GET_ERROR_AND_GOTO(os_copy_from_user(filename_buf,filename,OS_VFS_PATH_MAX),ret,err);
    filename_buf[OS_VFS_PATH_MAX] = '\0';
    os_size_t ret_fd;
    OS_ERR_GET_ERROR_AND_GOTO(os_file_fdid_create(&ret_fd),ret,err);
    os_file_fd_p new_fd_obj;
    OS_ASSERT((new_fd_obj = os_file_get_fd_by_fdid(ret_fd)) != OS_NULL);
    OS_ERR_GET_ERROR_AND_GOTO(os_file_open(new_fd_obj,filename_buf,flags),ret,open_err);
    os_memory_free(filename_buf);
    return OS_ERR_OK;

open_err:
    os_file_fdid_remove(ret_fd);
err:
    os_memory_free(filename_buf);
    return ret;
}

os_ssize_t os_syscall_close(struct TrapFrame *regs,os_size_t fd)
{
    os_file_fd_p fd_obj = os_file_get_fd_by_fdid(fd);
    OS_ERR_RETURN_ERROR(fd_obj == OS_NULL,-OS_ERR_EINVAL);
    return os_file_close(fd_obj);
}

os_ssize_t os_syscall_getdents64(struct TrapFrame *regs,os_size_t fd,os_size_t buf,os_size_t len)
{
    os_file_fd_p fd_obj = os_file_get_fd_by_fdid(fd);
    OS_ERR_RETURN_ERROR(fd_obj == OS_NULL,-OS_ERR_EINVAL);
    OS_ERR_RETURN_ERROR(len < sizeof(os_dirent_t),-OS_ERR_EINVAL);
    void *kmem = os_memory_alloc(len);
    OS_ERR_RETURN_ERROR(kmem == OS_NULL,-OS_ERR_ENOMEM);
    os_err_t read_dir_ret = os_file_readdir(fd_obj,kmem,len / sizeof(os_dirent_t));
    os_err_t ret = OS_ERR_OK;

    if(read_dir_ret >= OS_ERR_OK)
    {
        read_dir_ret *= sizeof(os_dirent_t);
        ret = os_copy_to_user(buf,kmem,len);

        if(ret == OS_ERR_OK)
        {
            ret = read_dir_ret;
        }
    }

    os_memory_free(kmem);
    return ret;
}

os_ssize_t os_syscall_read(struct TrapFrame *regs,os_size_t fd,os_size_t buf,os_size_t count)
{
    os_file_fd_p fd_obj = os_file_get_fd_by_fdid(fd);
    OS_ERR_RETURN_ERROR(fd_obj == OS_NULL,-OS_ERR_EINVAL);
    char *kbuf = os_memory_alloc(count);
    OS_ERR_RETURN_ERROR(fd_obj == OS_NULL,-OS_ERR_ENOMEM);
    os_err_t ret = os_file_read(fd_obj,kbuf,count);

    if(ret == OS_ERR_OK)
    {
        ret = os_copy_to_user(buf,kbuf,count);
    }

    os_memory_free(kbuf);
    return ret;
}

os_ssize_t os_syscall_write(struct TrapFrame *regs,os_size_t fd,os_size_t buf,os_size_t count)
{
    os_file_fd_p fd_obj = os_file_get_fd_by_fdid(fd);
    OS_ERR_RETURN_ERROR(fd_obj == OS_NULL,-OS_ERR_EINVAL);
    char *kbuf = os_memory_alloc(count);
    OS_ERR_RETURN_ERROR(fd_obj == OS_NULL,-OS_ERR_ENOMEM);
    os_err_t ret = os_copy_from_user(kbuf,buf,count);

    if(ret == OS_ERR_OK)
    {
        ret = os_file_write(fd_obj,kbuf,count);
    }

    os_memory_free(kbuf);
    return ret;
}

os_ssize_t os_syscall_linkat(struct TrapFrame *regs,os_size_t olddirfd,os_size_t oldpath,os_size_t newdirfd,os_size_t newpath,os_size_t flags)
{
    return -OS_ERR_EIO;
}

os_ssize_t os_syscall_unlinkat(struct TrapFrame *regs,os_size_t dirfd,os_size_t path,os_size_t flags)
{
    return -OS_ERR_EIO;
}

os_ssize_t os_syscall_mkdirat(struct TrapFrame *regs,os_size_t dirfd,os_size_t path,os_size_t mode)
{
    return -OS_ERR_EIO;
}

os_ssize_t os_syscall_umount2(struct TrapFrame *regs,os_size_t special,os_size_t flag)
{
    return -OS_ERR_EIO;
}

os_ssize_t os_syscall_mount(struct TrapFrame *regs,os_size_t special,os_size_t dir,os_size_t fstype,os_size_t flags,os_size_t data)
{
    return -OS_ERR_EIO;
}

os_ssize_t os_syscall_fstat(struct TrapFrame *regs,os_size_t fd,os_size_t kst)
{
    os_file_fd_p fd_obj = os_file_get_fd_by_fdid(fd);
    OS_ERR_RETURN_ERROR(fd_obj == OS_NULL,-OS_ERR_EINVAL);
    os_file_state_t stat;
    OS_ERR_GET_ERROR_AND_RETURN(os_vfs_stat(fd_obj -> fnode -> path,&stat));
    os_copy_to_user(kst,&stat,sizeof(stat));
    return OS_ERR_OK;
}

void arch_task_clone_stack_frame_init(struct TrapFrame *regs,os_task_t *task,os_size_t new_sp);

/*!
 * 用于Clone时拷贝页面映射并拷贝所有的页面数据，暂时不支持写时复制机制
 * @param task
 * @return
 */
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

os_ssize_t os_syscall_wait4(struct TrapFrame *regs,os_size_t pid,os_size_t status,os_size_t options)
{
    return 0;
}


os_ssize_t os_syscall_exit(struct TrapFrame *regs,os_ssize_t ec)
{
    OS_ENTER_CRITICAL_AREA();
    os_task_get_current_task() -> exit_code = ec;
    os_task_get_current_task() -> task_state = OS_TASK_STATE_STOPPED;
    OS_LEAVE_CRITICAL_AREA();
    os_task_schedule();
    return OS_ERR_OK;
}

os_ssize_t os_syscall_getpid(struct TrapFrame *regs)
{
    return (os_ssize_t)os_task_get_current_task() -> pid;
}

os_ssize_t os_syscall_getppid(struct TrapFrame *regs)
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

os_ssize_t os_syscall_brk(struct TrapFrame *regs,os_size_t brk)
{
    return 0;
}

os_ssize_t os_syscall_munmap(struct TrapFrame *regs,os_size_t start,os_size_t len)
{
    return 0;
}

os_ssize_t os_syscall_mmap(struct TrapFrame *regs,os_size_t start,os_size_t len,os_size_t prot,os_size_t flags,os_size_t fd,os_size_t off)
{
    return 0;
}

os_ssize_t os_syscall_times(struct TrapFrame *regs,os_size_t tms)
{
    return 0;
}

os_ssize_t os_syscall_uname(struct TrapFrame *regs,os_size_t uts)
{
    return 0;
}

os_ssize_t os_syscall_sched_yield(struct TrapFrame *regs)
{
    os_task_yield();
    return OS_ERR_OK;
}

os_ssize_t os_syscall_gettimeofday(struct TrapFrame *regs,os_size_t ts)
{
    return 0;
}

os_size_t os_syscall_nanosleep(struct TrapFrame *regs,os_size_t req,os_size_t rem)
{
    return 0;
}