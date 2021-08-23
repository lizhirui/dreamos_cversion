/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 * 2021-07-09     lizhirui     add fd_table support
 */

// @formatter:off
#ifndef __OS_FILE_H__
#define __OS_FILE_H__

    #include <dreamos.h>

    //文件状态结构体
    typedef struct os_file_state
    {
        os_size_t type;//类型
        os_size_t mode;//模式
        os_size_t size;//大小
        os_time_t access_time;//最后访问时间
        os_time_t modification_time;//最后修改时间
        os_time_t creation_time;//最后创建时间
    }os_file_state_t,*os_file_state_p;

    typedef struct os_file_fd os_file_fd_t,*os_file_fd_p;

    //目录项结构体
    typedef struct os_dirent
    {
        char name[OS_VFS_PATH_MAX + 1];//名称
        os_size_t type;//类型
    }os_dirent_t,*os_dirent_p;

    //文件操作函数集结构体
    typedef struct os_file_ops
    {
        /*!
         * 文件打开
         * @param fd 文件描述符结构体指针
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*open)(os_file_fd_p fd);
        /*!
         * 文件关闭
         * @param fd 文件描述符结构体指针
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*close)(os_file_fd_p fd);
        /*!
         * 文件ioctl
         * @param fd 文件描述符结构体指针
         * @param cmd 命令
         * @param arg 参数
         * @return 一般情况下成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*ioctl)(os_file_fd_p fd,os_size_t cmd,os_size_t arg);
        /*!
         * 文件读取
         * @param fd 文件描述符结构体指针
         * @param buf 数据缓冲区
         * @param size 数据大小
         * @return 成功返回读取的字节数，失败返回负数错误码
         */
        os_err_t (*read)(os_file_fd_p fd,void *buf,os_size_t size);
        /*!
         * 文件写入
         * @param fd 文件描述符结构体指针
         * @param buf 数据缓冲区
         * @param size 数据大小
         * @return 成功写入的字节数，失败返回负数错误码
         */
        os_err_t (*write)(os_file_fd_p fd,const void *buf,os_size_t size);
        /*!
         * 文件flush
         * @param fd 文件描述符结构体指针
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*flush)(os_file_fd_p fd);
        /*!
         * 文件读写指针调整
         * @param fd 文件描述符结构体指针
         * @param offset 相对于当前位置的偏移量
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*lseek)(os_file_fd_p fd,os_size_t offset);
        /*!
         * 读取目录项
         * @param fd 文件描述符结构体指针
         * @param entry 返回的目录项结构体指针
         * @param count 要读取的目录项数
         * @return 成功返回读取的目录项数，失败返回负数错误码
         */
        os_err_t (*readdir)(os_file_fd_p fd,os_dirent_p entry,os_size_t count);
    }os_file_ops_t,*os_file_ops_p;

    //文件节点结构体
    typedef struct os_file_node
    {
        char path[OS_VFS_PATH_MAX + 1];//文件路径
        os_size_t type;//类型
        os_size_t size;//大小
        os_vfs_mp_p mp;//关联的挂载点结构体指针
        os_file_ops_p ops;//文件操作函数集结构体指针
        os_size_t refcnt;//引用数
        void *priv_data;//私有数据
        os_list_node_t node;//列表节点
        os_mutex_t lock;//锁
    }os_file_node_t,*os_file_node_p;

    //文件描述符结构体
    struct os_file_fd
    {
        os_file_node_p fnode;//关联的文件节点结构体指针
        os_size_t open_flag;//打开标志
        os_size_t pos;//当前读写指针位置
        os_list_node_t node;//列表节点
        os_mutex_t lock;//锁
    };

    //文件描述符表结构体
    typedef struct os_file_fd_table
    {
        os_bitmap_t fd_bitmap;//文件描述符位图
        os_hashmap_t fd_hashmap;//文件描述符哈希表
        os_list_node_t fd_list;//文件描述符列表
        os_size_t refcnt;//引用数
        os_mutex_t lock;//锁
    }os_file_fd_table_t,*os_file_fd_table_p;

    os_file_fd_p os_file_get_fd_by_fdid(os_size_t fdid);
    os_err_t os_file_fdid_create(os_size_t *fdid);
    void os_file_fdid_remove(os_size_t fdid);
    os_file_fd_table_p os_file_fd_table_create();
    os_file_fd_table_p os_file_fd_table_soft_copy(os_file_fd_table_p fd_table);
    void os_file_fd_table_remove(os_file_fd_table_p fd_table);

    os_err_t os_file_open(os_file_fd_p fd,const char *path,os_size_t open_flag);
    os_err_t os_file_close(os_file_fd_p fd);
    os_err_t os_file_ioctl(os_file_fd_p fd,os_size_t cmd,os_size_t arg);
    os_err_t os_file_read(os_file_fd_p fd,void *buf,os_size_t size);
    os_err_t os_file_write(os_file_fd_p fd,const void *buf,os_size_t size);
    os_err_t os_file_flush(os_file_fd_p fd);
    os_err_t os_file_lseek(os_file_fd_p fd,os_size_t offset);
    os_err_t os_file_readdir(os_file_fd_p fd,os_dirent_p entry,os_size_t count);
    void os_file_init();

    //一般文件
    #define OS_FILE_TYPE_REGULAR 0
    //套接字文件
    #define OS_FILE_TYPE_SOCKET 1
    //目录文件
    #define OS_FILE_TYPE_DIRECTORY 2
    #define OS_FILE_TYPE_USER 3
    //设备文件
    #define OS_FILE_TYPE_DEVICE 4

    //只读
    #define OS_FILE_FLAG_RDONLY	    00000000
    //只写
    #define OS_FILE_FLAG_WRONLY	    00000001
    //读写
    #define OS_FILE_FLAG_RDWR		00000002
    //创建
    #define OS_FILE_FLAG_CREAT		00000100
    #define OS_FILE_FLAG_EXCL		00000200
    #define OS_FILE_FLAG_NOCTTY	    00000400
    //清空
    #define OS_FILE_FLAG_TRUNC		00001000
    //追加
    #define OS_FILE_FLAG_APPEND	    00002000
    #define OS_FILE_FLAG_NONBLOCK	00004000
    #define OS_FILE_FLAG_DSYNC		00010000
    #define OS_FILE_FLAG_FASYNC		00020000
    #define OS_FILE_FLAG_DIRECT	    00040000
    #define OS_FILE_FLAG_LARGEFILE	00100000
    //目录
    #define OS_FILE_FLAG_DIRECTORY	00200000
    #define OS_FILE_FLAG_NOFOLLOW	00400000
    #define OS_FILE_FLAG_NOATIME	01000000
    #define OS_FILE_FLAG_CLOEXEC	02000000

#endif