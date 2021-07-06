/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 */

#ifndef __OS_FILE_H__
#define __OS_FILE_H__

    #include <dreamos.h>

    typedef struct os_file_state
    {
        os_size_t type;
        os_size_t mode;
        os_size_t size;
        os_time_t access_time;
        os_time_t modification_time;
        os_time_t creation_time;
    }os_file_state_t,*os_file_state_p;

    typedef struct os_file_fd os_file_fd_t,*os_file_fd_p;

    typedef struct os_dirent
    {
        char name[OS_VFS_PATH_MAX + 1];
        os_size_t type;
    }os_dirent_t,*os_dirent_p;

    typedef struct os_file_ops
    {
        os_err_t (*open)(os_file_fd_p fd);
        os_err_t (*close)(os_file_fd_p fd);
        os_err_t (*ioctl)(os_file_fd_p fd,os_size_t cmd,os_size_t arg);
        os_err_t (*read)(os_file_fd_p fd,void *buf,os_size_t size);
        os_err_t (*write)(os_file_fd_p fd,const void *buf,os_size_t size);
        os_err_t (*flush)(os_file_fd_p fd);
        os_err_t (*lseek)(os_file_fd_p fd,os_size_t offset);
        os_err_t (*readdir)(os_file_fd_p fd,os_dirent_p entry,os_size_t count);
    }os_file_ops_t,*os_file_ops_p;

    typedef struct os_file_node
    {
        char path[OS_VFS_PATH_MAX + 1];
        os_size_t type;
        os_size_t size;
        os_vfs_mp_p mp;
        os_file_ops_p ops;
        os_size_t refcnt;
        void *priv_data;
        os_list_node_t node;
        os_mutex_t lock;
    }os_file_node_t,*os_file_node_p;

    struct os_file_fd
    {
        os_file_node_p fnode;
        os_size_t open_flag;
        os_size_t pos;
        os_mutex_t lock;
    };

    os_err_t os_file_open(os_file_fd_p fd,const char *path,os_size_t open_flag);
    os_err_t os_file_close(os_file_fd_p fd);
    os_err_t os_file_ioctl(os_file_fd_p fd,os_size_t cmd,os_size_t arg);
    os_err_t os_file_read(os_file_fd_p fd,void *buf,os_size_t size);
    os_err_t os_file_write(os_file_fd_p fd,const void *buf,os_size_t size);
    os_err_t os_file_flush(os_file_fd_p fd);
    os_err_t os_file_lseek(os_file_fd_p fd,os_size_t offset);
    os_err_t os_file_readdir(os_file_fd_p fd,os_dirent_p entry,os_size_t count);
    void os_file_init();

    #define OS_FILE_TYPE_REGULAR 0
    #define OS_FILE_TYPE_SOCKET 1
    #define OS_FILE_TYPE_DIRECTORY 2
    #define OS_FILE_TYPE_USER 3
    #define OS_FILE_TYPE_DEVICE 4

    #define OS_FILE_FLAG_RDONLY	    00000000
    #define OS_FILE_FLAG_WRONLY	    00000001
    #define OS_FILE_FLAG_RDWR		00000002
    #define OS_FILE_FLAG_CREAT		00000100
    #define OS_FILE_FLAG_EXCL		00000200
    #define OS_FILE_FLAG_NOCTTY	    00000400
    #define OS_FILE_FLAG_TRUNC		00001000
    #define OS_FILE_FLAG_APPEND	    00002000
    #define OS_FILE_FLAG_NONBLOCK	00004000
    #define OS_FILE_FLAG_DSYNC		00010000
    #define OS_FILE_FLAG_FASYNC		00020000
    #define OS_FILE_FLAG_DIRECT	    00040000
    #define OS_FILE_FLAG_LARGEFILE	00100000
    #define OS_FILE_FLAG_DIRECTORY	00200000
    #define OS_FILE_FLAG_NOFOLLOW	00400000
    #define OS_FILE_FLAG_NOATIME	01000000
    #define OS_FILE_FLAG_CLOEXEC	02000000

#endif