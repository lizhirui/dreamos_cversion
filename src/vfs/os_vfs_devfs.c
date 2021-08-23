/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-09     lizhirui     the first version
 */

// @formatter:off
#include <dreamos.h>

static os_err_t devfs_mount(os_vfs_mp_p mp,os_size_t mount_flag,void *priv_data)
{
    return OS_ERR_OK;
}

static os_err_t devfs_unmount(os_vfs_mp_p mp)
{
    return OS_ERR_OK;
}

static os_err_t devfs_mkfs(os_vfs_p fs,os_device_p dev)
{
    return -OS_ERR_EIO;
}

static os_err_t devfs_statfs(os_vfs_mp_p mp,os_vfs_state_p state)
{
    return -OS_ERR_EIO;
}

static os_err_t devfs_unlink(os_vfs_mp_p mp,const char *path)
{
    return -OS_ERR_EIO;
}

static os_err_t devfs_stat(os_vfs_mp_p mp,const char *path,os_file_state_p state)
{
    os_device_p dev = os_device_find(path + mp -> subpath_offset);

    if(dev == OS_NULL)
    {
        return -OS_ERR_ENOENT;
    }

    state -> type = OS_FILE_TYPE_DEVICE;
    state -> size = 0;
    return OS_ERR_OK;
}

static os_err_t devfs_rename(os_vfs_mp_p mp,const char *old_path,const char *new_path)
{
    return -OS_ERR_EIO;
}

static os_err_t devfs_open(os_file_fd_p fd)
{
    if(fd -> open_flag & (OS_FILE_FLAG_CREAT | OS_FILE_FLAG_APPEND | OS_FILE_FLAG_TRUNC))
    {
        return -OS_ERR_EINVAL;
    }

    if(fd -> open_flag & OS_FILE_FLAG_DIRECTORY)
    {
        return -OS_ERR_ENOTDIR;
    }

    OS_ASSERT(fd != OS_NULL);
    OS_ASSERT(fd -> fnode != OS_NULL);

    os_device_p dev = os_device_find(fd -> fnode -> path + fd -> fnode -> mp -> subpath_offset);

    if(dev == OS_NULL)
    {
        return -OS_ERR_ENOENT;
    }

    OS_ERR_GET_ERROR_AND_RETURN(os_device_op_open(dev,fd -> open_flag));

    if(fd -> fnode -> refcnt >= 1)
    {
        fd -> pos = 0;
        return OS_ERR_OK;
    }

    fd -> pos = 0;
    fd -> fnode -> type = OS_FILE_TYPE_DEVICE;
    fd -> fnode -> size = 0;
    fd -> fnode -> priv_data = (void *)dev;
    return OS_ERR_OK;
}

static os_err_t devfs_close(os_file_fd_p fd)
{
    os_device_p dev = (os_device_p)fd -> fnode -> priv_data;
    return os_device_op_close(dev);
}

static os_err_t devfs_ioctl(os_file_fd_p fd,os_size_t cmd,os_size_t arg)
{
    os_device_p dev = (os_device_p)fd -> fnode -> priv_data;
    return os_device_op_ioctl(dev,cmd,arg);
}

static os_err_t devfs_read(os_file_fd_p fd,void *buf,os_size_t size)
{
    os_device_p dev = (os_device_p)fd -> fnode -> priv_data;
    return os_device_op_read(dev,buf,fd -> pos,size);
}

static os_err_t devfs_write(os_file_fd_p fd,const void *buf,os_size_t size)
{
    os_device_p dev = (os_device_p)fd -> fnode -> priv_data;
    return os_device_op_write(dev,buf,fd -> pos,size);
}

static os_err_t devfs_flush(os_file_fd_p fd)
{
    return -OS_ERR_EIO;
}

static os_err_t devfs_lseek(os_file_fd_p fd,os_size_t offset)
{
    fd -> pos = offset;
    return OS_ERR_OK;
}

static os_err_t devfs_readdir(os_file_fd_p fd,os_dirent_p entry,os_size_t count)
{
    os_size_t i;

    if(count == 0)
    {
        return -OS_ERR_EINVAL;
    }

    os_list_node_p list_root = os_device_get_list();
    os_list_node_p cur_node = list_root;
    cur_node = cur_node -> next;

    for(i = 0;i < fd -> pos;i++)
    {
        if(cur_node -> next == list_root)
        {
            fd -> pos = i;
            break;
        }

        cur_node = cur_node -> next;
    }

    for(i = 0;(i < count) && (fd -> pos < fd -> fnode -> size);i++,fd -> pos++)
    {
        if(cur_node -> next == list_root)
        {
            break;
        }

        os_device_p dev = os_container_of(cur_node,os_device_t,node);
        os_strcpy(entry[i].name,dev -> name);
        entry[i].type = OS_FILE_TYPE_DEVICE;
    }

    return i;
}

static const os_file_ops_t devfs_file_ops =
{
    .open = devfs_open,
    .close = devfs_close,
    .ioctl = devfs_ioctl,
    .read = devfs_read,
    .write = devfs_write,
    .flush = devfs_flush,
    .lseek = devfs_lseek,
    .readdir = devfs_readdir
};

static const os_vfs_ops_t devfs_vfs_ops =
{
    .mount = devfs_mount,
    .unmount = devfs_unmount,
    .mkfs = devfs_mkfs,
    .statfs = devfs_statfs,
    .unlink = devfs_unlink,
    .stat = devfs_stat,
    .rename = devfs_rename,
    .file_ops = (os_file_ops_p)&devfs_file_ops
};

static const os_vfs_t devfs_vfs = 
{
    .name = "devfs",
    .flag = 0,
    .ops = (os_vfs_ops_p)&devfs_vfs_ops,
};

os_err_t os_vfs_devfs_init()
{
    return os_vfs_register((os_vfs_p)&devfs_vfs);
}