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

    //VFS文件系统状态结构体
    typedef struct os_vfs_state
    {
        os_size_t block_size;//块大小
        os_size_t block_count;//总块数量
        os_size_t block_free;//剩余块数量
    }os_vfs_state_t,*os_vfs_state_p;

    //VFS文件系统操作函数集结构体
    typedef struct os_vfs_ops
    {
        /*!
         * 文件系统挂载
         * @param mp 挂载点结构体指针
         * @param mount_flag 挂载标志，参考OS_FILE_FLAG_开头的标志
         * @param priv_data 私有数据
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*mount)(os_vfs_mp_p mp,os_size_t mount_flag,void *priv_data);
        /*!
         * 文件系统卸载
         * @param mp 挂载点结构体指针
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*unmount)(os_vfs_mp_p mp);
        /*!
         * 文件系统格式化
         * @param fs 文件系统结构体指针
         * @param dev 设备名
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*mkfs)(os_vfs_p fs,os_device_p dev);
        /*!
         * 获取文件系统状态
         * @param mp 挂载点结构体指针
         * @param state 返回的文件系统状态
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*statfs)(os_vfs_mp_p mp,os_vfs_state_p state);
        /*!
         * 删除文件
         * @param mp 挂载点结构体指针
         * @param path 文件路径
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*unlink)(os_vfs_mp_p mp,const char *path);
        /*!
         * 获取文件状态
         * @param mp 挂载点结构体指针
         * @param path 文件路径
         * @param state 返回的文件系统状态
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*stat)(os_vfs_mp_p mp,const char *path,os_file_state_p state);
        /*!
         * 重命名文件
         * @param mp 挂载点结构体指针
         * @param old_path 旧路径
         * @param new_path 新路径
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*rename)(os_vfs_mp_p mp,const char *old_path,const char *new_path);
        os_file_ops_p file_ops;
    }os_vfs_ops_t,*os_vfs_ops_p;

    //VFS文件系统信息结构体
    struct os_vfs
    {
        char *name;//文件系统名称
        os_size_t flag;//文件系统标志
        os_vfs_ops_p ops;//操作函数集
        os_list_node_t node;//链表节点
        os_mutex_t lock;//锁
        os_size_t mount_refcnt;//挂载引用数，指示有多少挂载点依赖该结构体
    };

    //VFS挂载点结构体
    struct os_vfs_mp
    {
        os_vfs_p fs;//关联的文件系统结构体指针
        char path[OS_VFS_PATH_MAX + 1];//挂载点路径（已经过正规化）
        os_size_t subpath_offset;//子路径偏移（即完整路径从该偏移开始属于该挂载点内的路径）
        os_device_p dev;//关联设备结构体指针
        const char *dev_path;//设备路径
        os_size_t mount_flag;//挂载标志，参考OS_FILE_FLAG_开头的标志
        os_list_node_t node;//链表节点
        os_mutex_t lock;//锁
        os_size_t mp_cnt;//指示有多少子挂载点
        os_size_t open_file_cnt;//指示该挂载点有多少打开的文件
        void *priv_data;//私有数据
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
    #include <vfs/os_vfs_devfs.h>

#endif