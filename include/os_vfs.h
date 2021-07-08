/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 */

// @formatter:off
#ifndef __OS_VFS_H__
#define __OS_VFS_H__

    #include <dreamos.h>

    typedef struct os_vfs os_vfs_t,*os_vfs_p;
    typedef struct os_vfs_mp os_vfs_mp_t,*os_vfs_mp_p;

    #include <os_file.h>

    typedef struct os_vfs_state
    {
        os_size_t block_size;
        os_size_t block_count;
        os_size_t block_free;
    }os_vfs_state_t,*os_vfs_state_p;

    typedef struct os_vfs_ops
    {
        os_err_t (*mount)(os_vfs_mp_p mp,os_size_t mount_flag,void *priv_data);
        os_err_t (*unmount)(os_vfs_mp_p mp);
        os_err_t (*mkfs)(os_vfs_p fs,os_device_p dev);
        os_err_t (*statfs)(os_vfs_mp_p mp,os_vfs_state_p state);
        os_err_t (*unlink)(os_vfs_mp_p mp,const char *path);
        os_err_t (*stat)(os_vfs_mp_p mp,const char *path,os_file_state_p state);
        os_err_t (*rename)(os_vfs_mp_p mp,const char *old_path,const char *new_path);
        os_file_ops_p file_ops;
    }os_vfs_ops_t,*os_vfs_ops_p;

    struct os_vfs
    {
        char *name;
        os_size_t flag;
        os_vfs_ops_p ops;
        os_list_node_t node;
        os_mutex_t lock;
        os_size_t mount_refcnt;
    };

    struct os_vfs_mp
    {
        os_vfs_p fs;
        char path[OS_VFS_PATH_MAX + 1];
        os_size_t subpath_offset;
        os_device_p dev;
        const char *dev_path;
        os_size_t mount_flag;
        os_list_node_t node;
        os_mutex_t lock;
        os_size_t mp_cnt;
        os_size_t open_file_cnt;
        void *priv_data;
    };

    void vfs_lock();
    void vfs_unlock();
    os_err_t os_vfs_normalize_path(const char *path,char *buf);
    os_vfs_mp_p os_vfs_find_mp_by_path(const char *path);
    os_err_t os_vfs_register(const os_vfs_p fs);
    os_err_t os_vfs_mount(const char *mount_path,const char *fs_name,const char *dev,os_size_t mount_flag,void *priv_data);
    os_err_t os_vfs_unmount(const char *mount_path);
    os_err_t os_vfs_mkfs(const char *fs_name,const char *dev);
    os_err_t os_vfs_statfs(const char *mount_path,os_vfs_state_p state);
    os_err_t os_vfs_unlink(const char *path);
    os_err_t os_vfs_stat(const char *path,os_file_state_p state);
    os_err_t os_vfs_rename(const char *old_path,const char *new_path);
    os_bool_t os_vfs_is_initialized();
    void os_vfs_init();

    #include <vfs/os_vfs_romfs.h>

#endif