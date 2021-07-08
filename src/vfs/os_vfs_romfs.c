/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-06     lizhirui     the first version
 */

// @formatter:off
#include <dreamos.h>

static inline os_bool_t romfs_check_dirent(os_vfs_romfs_dirent_p dirent)
{
    if((dirent -> type != OS_FILE_TYPE_REGULAR) 
        && (dirent -> type != OS_FILE_TYPE_DIRECTORY))
    {
        return OS_FALSE;
    }

    return OS_TRUE;
}

static os_vfs_romfs_dirent_p romfs_lookup(os_vfs_mp_p mp,os_vfs_romfs_dirent_p root_dirent,const char *path)
{
    const char *subpath;
    const char *subpath_end;
    os_vfs_romfs_dirent_p dirent;
    os_size_t subdir_entry_cnt,i;
    os_bool_t loop_continue = OS_TRUE;

    if(!romfs_check_dirent(root_dirent))
    {
        return OS_NULL;
    }

    path += mp -> subpath_offset;
    
    if(os_strcmp(path,"/") == 0)
    {
        return root_dirent;
    }
    
    dirent = (os_vfs_romfs_dirent_p)root_dirent -> data;
    subdir_entry_cnt = root_dirent -> size;

    subpath = path + 1;
    subpath_end = path + 2;

    while(*subpath_end && (*subpath_end != '/'))
    {
        subpath_end++;
    }

    while((dirent != OS_NULL) && loop_continue)
    {
        loop_continue = OS_FALSE;

        for(i = 0;i < subdir_entry_cnt;i++)
        {
            if(!romfs_check_dirent(&dirent[i]))
            {
                return OS_NULL;
            }

            if((os_strlen(dirent[i].name) == ((os_size_t)(subpath_end - subpath))) &&
                (os_memcmp(dirent[i].name,subpath,(os_size_t)(subpath_end - subpath)) == 0))
            {
                subdir_entry_cnt = dirent[i].size;

                if(!(*subpath_end))
                {
                    return &dirent[i];
                }

                subpath = subpath_end + 1;
                subpath_end += 2;

                while(*subpath_end && (*subpath_end != '/'))
                {
                    subpath_end++;
                }

                if(dirent[i].type == OS_FILE_TYPE_DIRECTORY)
                {
                    dirent = (os_vfs_romfs_dirent_p)dirent[i].data;
                    loop_continue = OS_TRUE;
                    break;
                }
                else
                {
                    return OS_NULL;
                }
            }
        }

        if(!loop_continue)
        {
            break;
        }
    }

    return OS_NULL;
}

static os_err_t romfs_mount(os_vfs_mp_p mp,os_size_t mount_flag,void *priv_data)
{
    if(priv_data == OS_NULL)
    {
        return -OS_ERR_EIO;
    }

    mp -> priv_data = priv_data;
    return OS_ERR_OK;
}

static os_err_t romfs_unmount(os_vfs_mp_p mp)
{
    return OS_ERR_OK;
}

static os_err_t romfs_mkfs(os_vfs_p fs,os_device_p dev)
{
    return -OS_ERR_EIO;
}

static os_err_t romfs_statfs(os_vfs_mp_p mp,os_vfs_state_p state)
{
    return -OS_ERR_EIO;
}

static os_err_t romfs_unlink(os_vfs_mp_p mp,const char *path)
{
    return -OS_ERR_EIO;
}

static os_err_t romfs_stat(os_vfs_mp_p mp,const char *path,os_file_state_p state)
{
    os_vfs_romfs_dirent_p root_dirent;
    os_vfs_romfs_dirent_p dirent;
    root_dirent = (os_vfs_romfs_dirent_p)mp -> priv_data;

    if(!romfs_check_dirent(root_dirent))
    {
        return -OS_ERR_EIO;
    }
    
    dirent = romfs_lookup(mp,root_dirent,path);

    if(dirent == OS_NULL)
    {
        return -OS_ERR_ENOENT;
    }

    state -> type = dirent -> type;
    state -> size = dirent -> size;
    return OS_ERR_OK;
}

static os_err_t romfs_rename(os_vfs_mp_p mp,const char *old_path,const char *new_path)
{
    return -OS_ERR_EIO;
}

static os_err_t romfs_open(os_file_fd_p fd)
{
    os_vfs_romfs_dirent_p root_dirent;
    os_vfs_romfs_dirent_p dirent;

    if(fd -> open_flag & (OS_FILE_FLAG_CREAT | OS_FILE_FLAG_WRONLY 
        | OS_FILE_FLAG_APPEND | OS_FILE_FLAG_TRUNC | OS_FILE_FLAG_RDWR))
    {
        return -OS_ERR_EINVAL;
    }

    OS_ASSERT(fd != OS_NULL);
    OS_ASSERT(fd -> fnode != OS_NULL);

    if(fd -> fnode -> refcnt >= 1)
    {
        if((fd -> fnode -> type == OS_FILE_TYPE_DIRECTORY) && !(fd -> open_flag & OS_FILE_FLAG_DIRECTORY))
        {
            return -OS_ERR_EISDIR;
        }

        if((fd -> fnode -> type != OS_FILE_TYPE_DIRECTORY) && (fd -> open_flag & OS_FILE_FLAG_DIRECTORY))
        {
            return -OS_ERR_ENOTDIR;
        }

        fd -> pos = 0;
        return 0;
    }

    root_dirent = (os_vfs_romfs_dirent_p)fd -> fnode -> mp -> priv_data;

    if(!romfs_check_dirent(root_dirent))
    {
        return -OS_ERR_EIO;
    }
    
    dirent = romfs_lookup(fd -> fnode -> mp,root_dirent,fd -> fnode -> path);

    if(dirent == OS_NULL)
    {
        return -OS_ERR_ENOENT;
    }

    if(dirent -> type == OS_FILE_TYPE_DIRECTORY)
    {
        if(!(fd -> open_flag & OS_FILE_FLAG_DIRECTORY))
        {
            return -OS_ERR_EISDIR;
        }

        fd -> fnode -> type = OS_FILE_TYPE_DIRECTORY;
    }
    else
    {
        if(fd -> open_flag & OS_FILE_FLAG_DIRECTORY)
        {
            return -OS_ERR_ENOTDIR;
        }

        fd -> fnode -> type = OS_FILE_TYPE_REGULAR;
    }

    fd -> pos = 0;
    fd -> fnode -> size = dirent -> size;
    fd -> fnode -> priv_data = (void *)dirent;
    return OS_ERR_OK;
}

static os_err_t romfs_close(os_file_fd_p fd)
{
    return OS_ERR_OK;
}

static os_err_t romfs_ioctl(os_file_fd_p fd,os_size_t cmd,os_size_t arg)
{
    return -OS_ERR_EIO;
}

static os_err_t romfs_read(os_file_fd_p fd,void *buf,os_size_t size)
{
    os_vfs_romfs_dirent_p dirent;

    dirent = (os_vfs_romfs_dirent_p)fd -> fnode -> priv_data;
    OS_ASSERT(dirent != OS_NULL);

    if(!romfs_check_dirent(dirent))
    {
        return -OS_ERR_EIO;
    }

    if(size > (fd -> fnode -> size - fd -> pos))
    {
        size = fd -> fnode -> size - fd -> pos;
    }

    if(size > 0)
    {
        os_memcpy(buf,&dirent -> data[fd -> pos],size);
    }

    fd -> pos += size;
    return size;
}

static os_err_t romfs_write(os_file_fd_p fd,const void *buf,os_size_t size)
{
    return -OS_ERR_EIO;
}

static os_err_t romfs_flush(os_file_fd_p fd)
{
    return -OS_ERR_EIO;
}

static os_err_t romfs_lseek(os_file_fd_p fd,os_size_t offset)
{
    if(offset <= fd -> fnode -> size)
    {
        fd -> pos = offset;
        return fd -> pos;
    }

    return -OS_ERR_EIO;
}

static os_err_t romfs_readdir(os_file_fd_p fd,os_dirent_p entry,os_size_t count)
{
    os_size_t i;
    os_vfs_romfs_dirent_p dirent;
    os_vfs_romfs_dirent_p sub_dirent;
    os_dirent_p sub_entry;

    dirent = (os_vfs_romfs_dirent_p)fd -> fnode -> priv_data;

    if(!romfs_check_dirent(dirent))
    {
        return -OS_ERR_EIO;
    }

    OS_ASSERT(dirent -> type == OS_FILE_TYPE_DIRECTORY);

    dirent = (os_vfs_romfs_dirent_p)dirent -> data;

    if(count == 0)
    {
        return -OS_ERR_EINVAL;
    }

    for(i = 0;(i < count) && (fd -> pos < fd -> fnode -> size);i++,fd -> pos++)
    {
        sub_dirent = &dirent[fd -> pos];
        sub_entry = &entry[i];

        sub_entry -> type = sub_dirent -> type;
        os_strcpy(sub_entry -> name,sub_dirent -> name);
    }

    return i;
}

static const os_file_ops_t romfs_file_ops =
{
    .open = romfs_open,
    .close = romfs_close,
    .ioctl = romfs_ioctl,
    .read = romfs_read,
    .write = romfs_write,
    .flush = romfs_flush,
    .lseek = romfs_lseek,
    .readdir = romfs_readdir
};

static const os_vfs_ops_t romfs_vfs_ops =
{
    .mount = romfs_mount,
    .unmount = romfs_unmount,
    .mkfs = romfs_mkfs,
    .statfs = romfs_statfs,
    .unlink = romfs_unlink,
    .stat = romfs_stat,
    .rename = romfs_rename,
    .file_ops = (os_file_ops_p)&romfs_file_ops
};

static const os_vfs_t romfs_vfs = 
{
    .name = "romfs",
    .flag = 0,
    .ops = (os_vfs_ops_p)&romfs_vfs_ops,
};

os_err_t os_vfs_romfs_init()
{
    return os_vfs_register((os_vfs_p)&romfs_vfs);
}