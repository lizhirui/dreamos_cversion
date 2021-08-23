/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 * 2021-07-06     lizhirui     add finer-grained lock
 * 2021-07-09     lizhirui     add fd_table support and open_flag check
 */

// @formatter:off
#include <dreamos.h>

static os_list_node_t os_file_list;//系统文件节点列表
static os_mutex_t os_file_global_lock;//文件管理器全局锁

/*!
 * 锁定文件管理器
 */
static void os_file_lock()
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&os_file_global_lock);
}

/*!
 * 解锁文件管理器
 */
static void os_file_unlock()
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_unlock(&os_file_global_lock);
}

/*!
 * 获取新的文件描述符ID
 * @param fdid 返回的文件描述符ID
 * @return 成功返回OS_ERR_OK，系统文件描述符ID全用完时返回-OS_ERR_EPERM
 */
static os_err_t os_file_get_new_fdid(os_size_t *fdid)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    OS_ASSERT(fdid != OS_NULL);
    os_file_fd_table_p fd_table = os_task_get_current_task() -> fd_table;
    os_mutex_lock(&fd_table -> lock);
    os_size_t ret = os_bitmap_find_some_ones(&fd_table -> fd_bitmap,0,1);

    if(ret == OS_NUMBER_MAX(os_size_t))
    {
        return -OS_ERR_EPERM;
    }

    *fdid = ret;
    os_bitmap_set_bit(&fd_table -> fd_bitmap,ret,0);
    os_mutex_unlock(&fd_table -> lock);
    return OS_ERR_OK;
}

/*!
 * 释放文件描述符ID
 * @param fdid 文件描述符ID
 */
static void os_file_release_fdid(os_size_t fdid)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    os_file_fd_table_p fd_table = os_task_get_current_task() -> fd_table;
    os_mutex_lock(&fd_table -> lock);
    os_bitmap_set_bit(&fd_table -> fd_bitmap,fdid,1);
    os_mutex_unlock(&fd_table -> lock);
}

/*!
 * 通过文件描述符ID获取文件描述符结构体指针
 * @param fdid 文件描述符ID
 * @return 文件描述符结构体指针
 */
os_file_fd_p os_file_get_fd_by_fdid(os_size_t fdid)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    os_file_fd_table_p fd_table = os_task_get_current_task() -> fd_table;
    os_mutex_lock(&fd_table -> lock);
    os_file_fd_p fd = OS_NULL;
    os_hashmap_get(&fd_table -> fd_hashmap,fdid,(void **)&fd);
    os_mutex_unlock(&fd_table -> lock);
    return fd;
}

/*!
 * 创建新的文件描述符
 * @param fdid 返回的文件描述符ID
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_file_fdid_create(os_size_t *fdid)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    OS_ASSERT(fdid != OS_NULL);
    OS_ERR_GET_ERROR_AND_RETURN(os_file_get_new_fdid(fdid));
    os_file_fd_p fd = os_memory_alloc(sizeof(os_file_fd_t));

    if(fd == OS_NULL)
    {
        os_file_release_fdid(*fdid);
        return -OS_ERR_ENOMEM;
    }

    os_file_fd_table_p fd_table = os_task_get_current_task() -> fd_table;
    os_mutex_lock(&fd_table -> lock);
    os_hashmap_set(&fd_table -> fd_hashmap,*fdid,(void *)fd);
    os_mutex_unlock(&fd_table -> lock);
    return OS_ERR_OK;
}

/*!
 * 销毁文件描述符
 * @param fdid 文件描述符ID
 */
void os_file_fdid_remove(os_size_t fdid)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    os_file_fd_table_p fd_table = os_task_get_current_task() -> fd_table;
    os_mutex_lock(&fd_table -> lock);
    os_file_fd_p fd = OS_NULL;

    if(os_hashmap_get(&fd_table -> fd_hashmap,fdid,(void **)&fd))
    {
        os_hashmap_remove_item(&fd_table -> fd_hashmap,fdid);
        os_list_node_remove(&fd -> node);
        os_memory_free(fd);
    }

    os_mutex_unlock(&fd_table -> lock);
}

/*!
 * 根据文件路径寻找文件节点结构体并返回其指针
 * @param path 文件路径
 * @return 成功返回文件节点结构体指针，失败返回OS_NULL
 */
static os_file_node_p os_file_fnode_find(const char *path)
{
    OS_ANNOTATION_NEED_VFS();
    os_file_lock();

    os_list_entry_foreach(os_file_list,os_file_node_t,node,entry,
    {
        if(os_strcmp(entry -> path,path) == 0)
        {
            os_file_unlock();
            return entry;
        }
    });

    os_file_unlock();
    return OS_NULL;
}

/*!
 * 创建文件描述符表，通常用于任务加载用户程序时
 * @return 成功返回文件描述符表结构体指针，失败返回OS_NULL
 */
os_file_fd_table_p os_file_fd_table_create()
{
    os_file_fd_table_p fd_table = os_memory_alloc(sizeof(os_file_fd_table_t));

    if(os_bitmap_create(&fd_table -> fd_bitmap,OS_MAX_OPEN_FILES,OS_NULL,1) != OS_ERR_OK)
    {
        os_memory_free((void *)fd_table);
        return OS_NULL;
    }

    if(os_hashmap_create(&fd_table -> fd_hashmap,OS_MAX_OPEN_FILES,OS_NULL) != OS_ERR_OK)
    {
        os_memory_free((void *)fd_table);
        return OS_NULL;
    }

    os_list_init(fd_table -> fd_list);
    fd_table -> refcnt = 1;
    os_mutex_init(&fd_table -> lock);
    return fd_table;
}

/*!
 * 拷贝一份文件描述符表，实现文件描述符表的共享，其原理是让描述符表的引用数+1
 * @param fd_table 文件描述符表结构体指针
 * @return 文件描述符表结构体指针
 */
os_file_fd_table_p os_file_fd_table_soft_copy(os_file_fd_table_p fd_table)
{
    os_mutex_lock(&fd_table -> lock);
    fd_table -> refcnt++;
    os_mutex_unlock(&fd_table -> lock);
    return fd_table;
}

/*!
 * 销毁文件描述符表，通常该函数只会在用户任务被清理时或用户任务通过execve加载新的ELF时
 * @param fd_table 文件描述符表结构体指针
 */
void os_file_fd_table_remove(os_file_fd_table_p fd_table)
{
    //只有当调度器初始化完成时，才对描述符表上锁，这个判断是用于调度器启动前初始化第一个任务时使用
    if(os_task_scheduler_is_initialized())
    {
        OS_ANNOTATION_NEED_VFS();
        OS_ANNOTATION_NEED_TASK_CONTEXT();
        os_mutex_lock(&fd_table -> lock);
    }

    //减少描述符表的引用数
    fd_table -> refcnt--;

    /*
     * 只有当引用数为0时才能销毁，引用数为0说明当前任务是使用这个描述符表的最后的任务，又由于该任务目前在execve或被清理的状态，
     * 因此任务在调用该函数时必然没有在试图申请使用描述符表，因此可以安全销毁
     */
    if(fd_table -> refcnt == 0)
    {
        //先销毁位图
        os_bitmap_remove(&fd_table -> fd_bitmap);

        //然后关闭文件描述符表中的每个文件并释放对应的节点内存
        os_list_entry_foreach_safe(fd_table -> fd_list,os_file_fd_t,node,entry,
        {
            os_list_node_remove(&entry -> node);
            os_file_close(entry);
            os_memory_free(&entry);
        });

        //最后释放hashmap并释放描述符表占用的内存
        os_hashmap_remove(&fd_table -> fd_hashmap);
        os_memory_free(fd_table);
    }

    if(os_task_scheduler_is_initialized())
    {
        os_mutex_unlock(&fd_table -> lock);
    }
}

/*!
 * 打开文件
 * @param fd 文件描述符结构体指针（不需要提前进行任何初始化工作）
 * @param path 文件路径
 * @param open_flag 文件打开标志，由内核OS_FILE_FLAG_开头的标志的位或序列构成
 * @return
 */
os_err_t os_file_open(os_file_fd_p fd,const char *path,os_size_t open_flag)
{
    OS_ANNOTATION_NEED_VFS();
    //因为需要对文件列表及文件系统进行操作，所以首先需要开启二者的全局锁
    os_file_lock();
    vfs_lock();

    os_err_t ret = OS_ERR_OK;

    //首先需要正规化路径，因此分配存放路径的内存空间
    char *path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,path_buf_alloc_err);
    //执行路径正规化操作
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(path,path_buf),ret,err);

    //根据路径获取文件系统节点
    os_vfs_mp_p mp = os_vfs_find_mp_by_path(path_buf);
    OS_ERR_SET_ERROR_AND_GOTO(mp == OS_NULL,ret,-OS_ERR_ENOENT,err);

    //试图获取文件节点，若获取失败，则分配一个新的文件节点，否则共享现有的文件节点
    os_file_node_p fnode = os_file_fnode_find(path);
    os_bool_t fnode_allocated = OS_FALSE;

    if(fnode == OS_NULL)
    {
        fnode_allocated = OS_TRUE;
        fnode = os_memory_alloc(sizeof(os_file_node_t));
        
        if(fnode == OS_NULL)
        {
            vfs_unlock();
            os_memory_free(path_buf);
            return -OS_ERR_ENOMEM;
        }

        fnode -> refcnt = 0;
        fnode -> mp = mp;
        fnode -> ops = mp -> fs -> ops -> file_ops;
        //初始化文件节点的锁
        os_mutex_init(&fnode -> lock);
        os_strcpy(fnode -> path,path_buf);
        //将新的文件节点挂入系统文件节点列表
        os_list_insert_tail(os_file_list,&fnode -> node);
    }

    //初始化文件描述符的锁
    os_mutex_init(&fd -> lock);
    //初始化其它标记
    fd -> pos = 0;
    fd -> open_flag = open_flag;
    fd -> fnode = fnode;

    //锁定文件系统并解锁全局锁以提高并发效率，该操作必须在vfs解锁前完成以保证原子性
    os_mutex_lock(&mp -> lock);
    vfs_unlock();
    //对文件节点加锁，该操作必须在文件管理器全局锁解锁前完成以保证原子性
    os_mutex_lock(&fnode -> lock);
    os_file_unlock();
    //执行文件的打开操作
    ret = mp -> fs -> ops -> file_ops -> open(fd);

    if(ret == OS_ERR_OK)
    {
        //若分配了一个新的文件节点，则将文件系统的文件打开数+1
        if(fnode_allocated)
        {
            mp -> open_file_cnt++;
        }

        //增加文件节点的引用数
        fnode -> refcnt++;
        os_mutex_unlock(&fnode -> lock);
    }
    else
    {
        if(fnode_allocated)
        {
            os_list_node_remove(&fnode -> node);
            os_memory_free(fnode);
        }
        else
        {
            os_mutex_unlock(&fnode -> lock);
        }
    }

    //最后对文件系统解锁，并记得释放掉用于正规化路径的缓冲区
    os_mutex_unlock(&mp -> lock);
    os_memory_free(path_buf);
    return ret;

err:
    os_memory_free(path_buf);
path_buf_alloc_err:
    vfs_unlock();
    os_file_unlock();
    return ret;
}

/*!
 * 关闭文件
 * @param fd 文件描述符结构体指针
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_file_close(os_file_fd_p fd)
{
    OS_ANNOTATION_NEED_VFS();
    //因为可能对文件列表进行操作，所以此处对文件管理器加全局锁
    os_file_lock();
    os_mutex_lock(&fd -> lock);//锁定该文件描述符
    //执行该文件描述符的关闭操作
    os_err_t ret = fd -> fnode -> ops -> close(fd);
    
    if(ret == OS_ERR_OK)
    {
        /*
         * 需要减少文件节点的引用数，若引用数变为0时需要销毁该文件节点，
         * 因为可能涉及到文件系统操作，需要先后对文件系统和文件节点加锁（顺序不可错，防止死锁）
         */
        os_mutex_lock(&fd -> fnode -> mp -> lock);
        os_mutex_lock(&fd -> fnode -> lock);
        fd -> fnode -> refcnt--;

        if(fd -> fnode -> refcnt == 0)
        {
            //引用数变为0时需要销毁文件节点
            fd -> fnode -> mp -> open_file_cnt--;
            os_list_node_remove(&fd -> fnode -> node);
            os_memory_free(fd -> fnode);
        }
        else
        {
            os_mutex_unlock(&fd -> fnode -> lock);
        }
        
        os_mutex_unlock(&fd -> fnode -> mp -> lock);
    }

    os_mutex_unlock(&fd -> lock);
    os_file_unlock();
    return ret;
}

/*!
 * 文件ioctl操作
 * @param fd 文件描述符结构体指针
 * @param cmd 命令
 * @param arg 参数
 * @return 成功一般返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_file_ioctl(os_file_fd_p fd,os_size_t cmd,os_size_t arg)
{
    OS_ANNOTATION_NEED_VFS();
    //对文件描述符和文件节点全部加锁，以防止ioctl需要访问这二者
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    os_err_t ret = fd -> fnode -> ops -> ioctl(fd,cmd,arg);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

/*!
 * 文件读取
 * @param fd 文件描述符结构体指针
 * @param buf 数据缓冲区
 * @param size 数据量（字节数）
 * @return 成功返回OS_ERR_OK，失败返回负数错误码，返回-OS_ERR_EACCESS可能是因为文件没有读取权限
 */
os_err_t os_file_read(os_file_fd_p fd,void *buf,os_size_t size)
{
    OS_ANNOTATION_NEED_VFS();
    //对文件描述符和文件节点全部加锁，以防止read需要访问这二者
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    //调用具体的读取函数前先进行权限检查
    os_err_t ret = (fd -> open_flag & OS_FILE_FLAG_WRONLY) ? -OS_ERR_EACCES : fd -> fnode -> ops -> read(fd,buf,size);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

/*!
 * 文件写入
 * @param fd 文件描述符结构体指针
 * @param buf 数据缓冲区
 * @param size 数据量（字节数）
 * @return 成功返回OS_ERR_OK，失败返回负数错误码，返回-OS_ERR_EACCESS可能是因为文件没有写入权限
 */
os_err_t os_file_write(os_file_fd_p fd,const void *buf,os_size_t size)
{
    OS_ANNOTATION_NEED_VFS();
    //对文件描述符和文件节点全部加锁，以防止write需要访问这二者
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    //调用具体的写入函数前先进行权限检查
    os_err_t ret = (!(fd -> open_flag & (OS_FILE_FLAG_WRONLY | OS_FILE_FLAG_RDWR))) ? -OS_ERR_EACCES : fd -> fnode -> ops -> write(fd,buf,size);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

/*!
 * 文件flush
 * @param fd 文件描述符结构体指针
 * @return 成功返回OS_ERR_OK，失败返回负数错误码，返回-OS_ERR_EACCESS可能是因为文件没有写入权限
 */
os_err_t os_file_flush(os_file_fd_p fd)
{
    OS_ANNOTATION_NEED_VFS();
    //对文件描述符和文件节点全部加锁，以防止flush需要访问这二者
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    //调用具体的flush函数前先进行权限检查
    os_err_t ret = (!(fd -> open_flag & (OS_FILE_FLAG_WRONLY | OS_FILE_FLAG_RDWR))) ? -OS_ERR_EACCES : fd -> fnode -> ops -> flush(fd);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

/*!
 * 移动文件读写指针
 * @param fd 文件描述符结构体指针
 * @param offset 成功返回OS_ERR_OK，失败返回负数错误码
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_file_lseek(os_file_fd_p fd,os_size_t offset)
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    os_err_t ret = fd -> fnode -> ops -> lseek(fd,offset);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

/*!
 * 读取目录项
 * @param fd 文件描述符结构体指针
 * @param entry 目录项结构体指针
 * @param count 要读取的目录项个数
 * @return 成功返回OS_ERR_OK，失败返回负数错误码，返回-OS_ERR_EACCESS可能是因为文件没有写入权限，
 * 返回-OS_ERR_EPERM可能是因为文件没有以目录标识打开
 */
os_err_t os_file_readdir(os_file_fd_p fd,os_dirent_p entry,os_size_t count)
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    os_err_t ret = (fd -> open_flag & OS_FILE_FLAG_WRONLY) ? -OS_ERR_EACCES :
        ((!(fd -> open_flag & OS_FILE_FLAG_DIRECTORY)) ? -OS_ERR_EPERM : fd -> fnode -> ops -> readdir(fd,entry,count));
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

/*!
 * 文件管理器初始化
 */
void os_file_init()
{
    os_list_init(os_file_list);
    os_mutex_init(&os_file_global_lock);
}