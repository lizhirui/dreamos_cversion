/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 */

#include <dreamos.h>

static os_list_node_t device_list;
static os_mutex_t os_device_global_lock;
static os_bool_t os_device_initialized = OS_FALSE;

static void os_device_lock()
{
    os_mutex_lock(&os_device_global_lock);
}

static void os_device_unlock()
{
    os_mutex_unlock(&os_device_global_lock);
}

os_device_p os_device_find(const char *name)
{
    OS_ANNOTATION_NEED_DEVICE();
    os_device_lock();

    os_list_entry_foreach(device_list,os_device_t,node,entry,
    {
        if(os_strcmp(entry -> name,name) == 0)
        {
            os_device_unlock();
            return entry;
        }
    });

    os_device_unlock();
    return OS_NULL;
}

os_err_t os_device_register(os_device_p dev)
{
    OS_ANNOTATION_NEED_DEVICE();

    OS_ERR_RETURN_ERROR(os_device_find(dev -> name) != OS_NULL,-OS_ERR_EINVAL);

    os_device_lock();
    os_list_insert_tail(device_list,&dev -> node);
    os_device_unlock();
    return OS_ERR_OK;
}

os_bool_t os_device_is_initialized()
{
    return os_device_initialized;
}

void os_device_init()
{
    os_list_init(device_list);
    os_mutex_init(&os_device_global_lock);
    os_device_initialized = OS_TRUE;
}