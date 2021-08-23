/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-08     lizhirui     the first version
 * 2021-07-09     lizhirui     add fd_table support
 */

// @formatter:off
#include <dreamos.h>

//架构相关的elf操作类型前置声明
#ifndef OS_ARCH64
    typedef os_elf_elf32_ehdr_t os_elf_ehdr_t;
    typedef os_elf_elf32_ehdr_p os_elf_ehdr_p;
    typedef os_elf_elf32_shdr_t os_elf_shdr_t;
    typedef os_elf_elf32_shdr_p os_elf_shdr_p;
    typedef os_elf_elf32_phdr_t os_elf_phdr_t;
    typedef os_elf_elf32_phdr_p os_elf_phdr_p;
    typedef os_elf_elf32_sym_t os_elf_sym_t;
    typedef os_elf_elf32_sym_p os_elf_sym_p;
    typedef os_elf_elf32_addr_t os_elf_addr_t;
    typedef os_elf_elf32_half_t os_elf_half_t;
    typedef os_elf_elf32_off_t os_elf_off_t;
    typedef os_elf_elf32_sword_t os_elf_sword_t;
    typedef os_elf_elf32_word_t os_elf_word_t;
    #define OS_ELF_ELFCLASS OS_ELF_ELFCLASS32
#else
    typedef os_elf_elf64_ehdr_t os_elf_ehdr_t;
    typedef os_elf_elf64_ehdr_p os_elf_ehdr_p;
    typedef os_elf_elf64_shdr_t os_elf_shdr_t;
    typedef os_elf_elf64_shdr_p os_elf_shdr_p;
    typedef os_elf_elf64_phdr_t os_elf_phdr_t;
    typedef os_elf_elf64_phdr_p os_elf_phdr_p;
    typedef os_elf_elf64_sym_t os_elf_sym_t;
    typedef os_elf_elf64_sym_p os_elf_sym_p;
    typedef os_elf_elf64_addr_t os_elf_addr_t;
    typedef os_elf_elf64_half_t os_elf_half_t;
    typedef os_elf_elf64_off_t os_elf_off_t;
    typedef os_elf_elf64_sword_t os_elf_sword_t;
    typedef os_elf_elf64_word_t os_elf_word_t;
    #define OS_ELF_ELFCLASS OS_ELF_ELFCLASS64
#endif

/*!
 * 加载ELF，执行该函数会同时导致为任务创建新的页表和文件符号表
 * @param path ELF路径
 * @param entry 返回ELF的入口地址
 * @return 成功返回OS_ERR_OK；若路径为OS_NULL或ELF头部信息存在错误，则返回-OS_ERR_EINVAL；若内存不足，则返回-OS_ERR_ENOMEM；同时也存在其它的错误情况和错误码
 */
os_err_t os_elf_load(const char *path,os_size_t *entry)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    OS_ANNOTATION_NEED_VFS();

    OS_ASSERT(entry != OS_NULL);

    os_file_fd_t fd;
    os_task_p task = os_task_get_current_task();

    os_elf_ehdr_t ehdr;
    os_elf_shdr_t shdr;
    os_elf_phdr_t phdr;
    os_size_t i;

    OS_ERR_RETURN_ERROR(path == OS_NULL,-OS_ERR_EINVAL);
    //打开文件
    OS_ERR_GET_ERROR_AND_RETURN(os_file_open(&fd,path,OS_FILE_FLAG_RDONLY));
    //读取ELF Header
    OS_ERR_GET_ERROR_AND_RETURN(os_file_lseek(&fd,0));
    OS_ERR_GET_ERROR_AND_RETURN(os_file_read(&fd,&ehdr,sizeof(ehdr)));
    //模数检查
    OS_ERR_RETURN_ERROR((ehdr.e_ident[OS_ELF_EI_MAG0] != OS_ELF_ELFMAG0) ||
                                 (ehdr.e_ident[OS_ELF_EI_MAG1] != OS_ELF_ELFMAG1) ||
                                 (ehdr.e_ident[OS_ELF_EI_MAG2] != OS_ELF_ELFMAG2) ||
                                 (ehdr.e_ident[OS_ELF_EI_MAG3] != OS_ELF_ELFMAG3),-OS_ERR_EINVAL);
    //类别检查
    OS_ERR_RETURN_ERROR(ehdr.e_ident[OS_ELF_EI_CLASS] != OS_ELF_ELFCLASS,-OS_ERR_EINVAL);
    //版本检查
    OS_ERR_RETURN_ERROR(ehdr.e_ident[OS_ELF_EI_VERSION] != 1,-OS_ERR_EINVAL);
    //可执行文件类型检查，必须为OS_ELF_ET_EXEC
    OS_ERR_RETURN_ERROR(ehdr.e_type != OS_ELF_ET_EXEC,-OS_ERR_EINVAL);
    //检查入口地址是否落在系统允许的用户程序地址范围内
    OS_ERR_RETURN_ERROR((ehdr.e_entry < OS_MMU_MEMORYMAP_USER_REAL_START) ||
                                 (ehdr.e_entry >= (OS_MMU_MEMORYMAP_USER_VTABLE_START +
                                 OS_MMU_MEMORYMAP_USER_VTABLE_SIZE)),-OS_ERR_EINVAL);

    //创建用户页表
    os_mmu_vtable_p vtable = os_memory_alloc(sizeof(os_mmu_vtable_t));
    os_err_t ret = OS_ERR_OK;
    OS_ERR_RETURN_ERROR(vtable == OS_NULL,-OS_ERR_ENOMEM);
    OS_ERR_GET_ERROR_AND_GOTO(os_mmu_vtable_create(vtable,OS_NULL,OS_MMU_MEMORYMAP_USER_VTABLE_START,OS_MMU_MEMORYMAP_USER_VTABLE_SIZE),ret,err);

    //检查ELF的Program Header大小是否正确
    OS_ERR_RETURN_ERROR(ehdr.e_phentsize != sizeof(phdr),-OS_ERR_EINVAL);

    //遍历Program Header加载程序和数据
    for(i = 0;i < ehdr.e_phnum;i++)
    {
        OS_ERR_GET_ERROR_AND_GOTO(os_file_lseek(&fd,ehdr.e_phoff + i * sizeof(phdr)),ret,other_err);
        OS_ERR_GET_ERROR_AND_RETURN(os_file_read(&fd,&phdr,sizeof(phdr)));

        //只处理需要加载到内存的Program Header
        if(phdr.p_type != OS_ELF_PT_LOAD)
        {
            continue;
        }

        //根据访存属性配置MMU属性
        os_mmu_pt_prot_t prot = OS_MMU_PROT_USER;

        if((phdr.p_flags & OS_ELF_PF_R) && (phdr.p_flags & OS_ELF_PF_W) && (phdr.p_flags & OS_ELF_PF_X))
        {
            OS_MMU_PROT_RWX(&prot);
        }
        else if((phdr.p_flags & OS_ELF_PF_R) && (phdr.p_flags & OS_ELF_PF_W))
        {
            OS_MMU_PROT_RW(&prot);
        }
        else if((phdr.p_flags & OS_ELF_PF_R) && (phdr.p_flags & OS_ELF_PF_X))
        {
            OS_MMU_PROT_RX(&prot);
        }
        else if(phdr.p_flags & OS_ELF_PF_R)
        {
            OS_MMU_PROT_RO(&prot);
        }
        else
        {
            ret = -OS_ERR_EINVAL;
            goto other_err;
        }

        //计算虚拟地址和地址空间大小
        os_size_t user_va = ALIGN_DOWN(phdr.p_vaddr,OS_MMU_PAGE_SIZE);
        os_size_t user_size = ALIGN_UP(phdr.p_memsz,OS_MMU_PAGE_SIZE);

        OS_ERR_SET_ERROR_AND_GOTO(phdr.p_align != OS_MMU_PAGE_SIZE,ret,-OS_ERR_EINVAL,other_err);
        //为虚拟地址空间分配物理内存并进行映射
        OS_ERR_GET_ERROR_AND_RETURN(os_mmu_create_mapping_auto(vtable,user_va,user_size,prot));

        //开始数据拷贝
        OS_ERR_GET_ERROR_AND_GOTO(os_file_lseek(&fd,phdr.p_offset),ret,other_err);

        os_size_t user_file_offset = phdr.p_offset;
        os_size_t user_file_size = phdr.p_filesz;
        os_size_t user_file_uplimit = user_file_offset + user_file_size;
        os_size_t user_mem_offset = phdr.p_vaddr;
        os_size_t cur_file_offset;
        os_size_t cur_mem_offset;

        for(cur_file_offset = user_file_offset,cur_mem_offset = user_mem_offset;cur_file_offset < user_file_uplimit;)
        {
            os_size_t size = MIN(user_file_uplimit - cur_file_offset,
            OS_MMU_PAGE_SIZE - OS_MMU_PAGE_OFFSET(cur_file_offset));
            void *kmem = os_mmu_user_va_to_kernel_va(vtable,cur_mem_offset);
            OS_ASSERT(kmem != OS_NULL);
            OS_ERR_GET_ERROR_AND_GOTO(os_file_lseek(&fd,cur_file_offset),ret,other_err);
            OS_ERR_GET_ERROR_AND_GOTO(os_file_read(&fd,kmem,size),ret,other_err);

            cur_file_offset += size;
            cur_mem_offset += size;
        }
    }

    //拷贝IO Mapping到页表
    os_mmu_io_mapping_copy(vtable);
    //拷贝Kernel Mappinp到页表
    os_mmu_kernel_mapping_copy(vtable);

    //将任务当前的页表引用数减一，若为0，则销毁该页表
    task -> vtable -> refcnt--;

    if(task -> vtable -> refcnt == 0)
    {
        os_mmu_vtable_remove(task -> vtable,OS_TRUE);
    }

    //将新页表引用数设置为1
    vtable -> refcnt = 1;

    //分配新的文件描述符表
    os_file_fd_table_p fd_table = os_file_fd_table_create();
    OS_ERR_SET_ERROR_AND_GOTO(fd_table == OS_NULL,ret,-OS_ERR_ENOMEM,other_err);
    os_file_fd_table_p old_fd_table = task -> fd_table;//临时备份旧的文件描述符表
    task -> fd_table = fd_table;//临时加载新表，若下面出错时，会还原为旧表
    os_size_t fdid;
    os_file_fd_p t_fd;

    //创建stdin，关联到OS_CONSOLE_DEVICE
    OS_ERR_GET_ERROR_AND_GOTO(os_file_fdid_create(&fdid),ret,fd_table_err);
    OS_ASSERT(fdid == 0);
    t_fd = os_file_get_fd_by_fdid(fdid);
    OS_ASSERT(t_fd != OS_NULL);
    OS_ERR_GET_ERROR_AND_GOTO(os_file_open(t_fd,OS_CONSOLE_DEVICE,OS_FILE_FLAG_RDONLY),ret,fd_table_err);

    //创建stdout，关联到OS_CONSOLE_DEVICE
    OS_ERR_GET_ERROR_AND_GOTO(os_file_fdid_create(&fdid),ret,fd_table_err);
    OS_ASSERT(fdid == 1);
    t_fd = os_file_get_fd_by_fdid(fdid);
    OS_ASSERT(t_fd != OS_NULL);
    OS_ERR_GET_ERROR_AND_GOTO(os_file_open(t_fd,OS_CONSOLE_DEVICE,OS_FILE_FLAG_WRONLY),ret,fd_table_err);

    //创建stderr，关联到OS_CONSOLE_DEVICE
    OS_ERR_GET_ERROR_AND_GOTO(os_file_fdid_create(&fdid),ret,fd_table_err);
    OS_ASSERT(fdid == 2);
    t_fd = os_file_get_fd_by_fdid(fdid);
    OS_ASSERT(t_fd != OS_NULL);
    OS_ERR_GET_ERROR_AND_GOTO(os_file_open(t_fd,OS_CONSOLE_DEVICE,OS_FILE_FLAG_WRONLY),ret,fd_table_err);

    //销毁旧文件描述符表
    os_file_fd_table_remove(old_fd_table);
    //设置任务可执行文件路径
    os_strcpy(task -> path,path);
    const char *filename = path;
    const char *buf = path;

    while(*buf)
    {
        if(*buf == '/')
        {
            filename = buf + 1;
        }

        buf++;
    }

    //更新任务名
    os_strcpy(task -> name,filename);
    task -> vtable = vtable;//设置任务页表
    os_mmu_switch(vtable);//切换到新的页表
    *entry = ehdr.e_entry;//返回任务的入口地址
    return OS_ERR_OK;

fd_table_err:
    os_file_fd_table_remove(fd_table);
    task -> fd_table = old_fd_table;
other_err:
    os_mmu_vtable_remove(vtable,OS_TRUE);
err:
    os_memory_free(vtable);
    return ret;
}
