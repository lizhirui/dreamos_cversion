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

    typedef struct os_device_ops
    {
        os_err_t (*init)(os_device_p dev);
        os_err_t (*open)(os_device_p dev,os_size_t open_flag);
        os_err_t (*close)(os_device_t dev);
        os_err_t (*read)(os_device_p dev,void *buf,os_size_t pos,os_size_t size);
        os_err_t (*write)(os_device_p dev,const void *buf,os_size_t pos,os_size_t size);
        os_err_t (*ioctl)(os_device_p dev,os_size_t cmd,os_size_t arg);
    }os_chardevice_ops_t,*os_device_ops_p;

    struct os_device
    {
        char *name;
        os_bool_t initialized;
        os_size_t flag;
        os_size_t open_flag;
        void *priv_data;
        os_list_node_t node;
        os_device_ops_p ops;
    };

    os_device_p os_device_find(const char *name);
    os_err_t os_device_register(os_device_p dev);
    os_bool_t os_device_is_initialized();
    void os_device_init();

    //#include <device/os_chardevice.h>

#endif