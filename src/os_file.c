/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 * 2021-07-06     lizhirui     add finer-grained lock
 */

#include <dreamos.h>

static os_list_node_t os_file_list;
static os_mutex_t os_file_global_lock;

static void os_file_lock()
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&os_file_global_lock);
}

static void os_file_unlock()
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_unlock(&os_file_global_lock);
}

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

os_err_t os_file_open(os_file_fd_p fd,const char *path,os_size_t open_flag)
{
    OS_ANNOTATION_NEED_VFS();
    os_file_lock();
    vfs_lock();

    os_err_t ret = OS_ERR_OK;
    
    char *path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,path_buf_alloc_err);
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(path,path_buf),ret,err);

    os_vfs_mp_p mp = os_vfs_find_mp_by_path(path_buf);
    OS_ERR_SET_ERROR_AND_GOTO(mp == OS_NULL,ret,-OS_ERR_ENOENT,err);

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
        os_mutex_init(&fnode -> lock);
        os_strcpy(fnode -> path,path_buf);
        os_list_insert_tail(os_file_list,&fnode -> node);
    }

    os_mutex_init(&fd -> lock);
    fd -> pos = 0;
    fd -> open_flag = open_flag;
    fd -> fnode = fnode;

    os_mutex_lock(&mp -> lock);
    vfs_unlock();
    os_mutex_lock(&fnode -> lock);
    os_file_unlock();
    ret = mp -> fs -> ops -> file_ops -> open(fd);

    if(ret == OS_ERR_OK)
    {
        if(fnode_allocated)
        {
            mp -> open_file_cnt++;
        }

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

os_err_t os_file_close(os_file_fd_p fd)
{
    OS_ANNOTATION_NEED_VFS();
    os_file_lock();
    os_mutex_lock(&fd -> lock);
    os_err_t ret = fd -> fnode -> ops -> close(fd);
    
    if(ret == OS_ERR_OK)
    {
        os_mutex_lock(&fd -> fnode -> mp -> lock);
        os_mutex_lock(&fd -> fnode -> lock);
        fd -> fnode -> refcnt--;

        if(fd -> fnode -> refcnt == 0)
        {
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

os_err_t os_file_ioctl(os_file_fd_p fd,os_size_t cmd,os_size_t arg)
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    os_err_t ret = fd -> fnode -> ops -> ioctl(fd,cmd,arg);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

os_err_t os_file_read(os_file_fd_p fd,void *buf,os_size_t size)
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    os_err_t ret = fd -> fnode -> ops -> read(fd,buf,size);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

os_err_t os_file_write(os_file_fd_p fd,const void *buf,os_size_t size)
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    os_err_t ret = fd -> fnode -> ops -> write(fd,buf,size);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

os_err_t os_file_flush(os_file_fd_p fd)
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    os_err_t ret = fd -> fnode -> ops -> flush(fd);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

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

os_err_t os_file_readdir(os_file_fd_p fd,os_dirent_p entry,os_size_t count)
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&fd -> lock);
    os_mutex_lock(&fd -> fnode -> lock);
    os_err_t ret = fd -> fnode -> ops -> readdir(fd,entry,count);
    os_mutex_unlock(&fd -> fnode -> lock);
    os_mutex_unlock(&fd -> lock);
    return ret;
}

void os_file_init()
{
    os_list_init(os_file_list);
    os_mutex_init(&os_file_global_lock);
}