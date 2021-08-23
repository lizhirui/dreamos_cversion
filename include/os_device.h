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
#ifndef __OS_DEVICE_H__
#define __OS_DEVICE_H__

    #include <dreamos.h>

    typedef struct os_device os_device_t,*os_device_p;

    //设备操作函数集结构体
    typedef struct os_device_ops
    {
        /*!
         * 设备初始化
         * @param dev 设备结构体指针
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*init)(os_device_p dev);
        /*!
         * 设备打开
         * @param dev 设备结构体指针
         * @param open_flag 打开标志
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*open)(os_device_p dev,os_size_t open_flag);
        /*!
         * 设备关闭
         * @param dev 设备结构体指针
         * @return 成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*close)(os_device_p dev);
        /*!
         * 设备读取
         * @param dev 设备结构体指针
         * @param buf 数据缓冲区
         * @param pos 读指针位置
         * @param size 数据大小
         * @return 成功返回读取的字节数，失败返回负数错误码
         */
        os_err_t (*read)(os_device_p dev,void *buf,os_size_t pos,os_size_t size);
        /*!
         * 设备写入
         * @param dev 设备结构体指针
         * @param buf 数据缓冲区
         * @param pos 读指针位置
         * @param size 数据大小
         * @return 成功返回写入的字节数，失败返回负数错误码
         */
        os_err_t (*write)(os_device_p dev,const void *buf,os_size_t pos,os_size_t size);
        /*!
         * 设备ioctl
         * @param dev 设备结构体指针
         * @param cmd 命令
         * @param arg 参数
         * @return 一般成功返回OS_ERR_OK，失败返回负数错误码
         */
        os_err_t (*ioctl)(os_device_p dev,os_size_t cmd,os_size_t arg);
    }os_device_ops_t,*os_device_ops_p;

    //设备结构体
    struct os_device
    {
        char *name;//设备名
        os_bool_t initialized;//指示设备是否已初始化
        os_size_t flag;//设备标志
        os_size_t open_flag;//设备打开标志，参见OS_FILE_FLAG_相关标志
        os_size_t refcnt;//引用数
        os_mutex_t lock;//锁
        void *priv_data;//私有数据
        os_list_node_t node;//列表节点
        os_device_ops_p ops;//设备操作函数集结构体指针
    };

    os_list_node_p os_device_get_list();
    os_device_p os_device_find(const char *name);
    os_err_t os_device_register(os_device_p dev);
    os_err_t os_device_op_init(os_device_p dev);
    os_err_t os_device_op_open(os_device_p dev,os_size_t open_flag);
    os_err_t os_device_op_close(os_device_p dev);
    os_err_t os_device_op_read(os_device_p dev,void *buf,os_size_t pos,os_size_t size);
    os_err_t os_device_op_write(os_device_p dev,const void *buf,os_size_t pos,os_size_t size);
    os_err_t os_device_op_ioctl(os_device_p dev,os_size_t cmd,os_size_t arg);
    os_bool_t os_device_is_initialized();
    void os_device_init();

    //#include <device/os_chardevice.h>

#endif