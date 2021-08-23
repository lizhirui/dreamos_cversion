/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 * 2021-07-09     lizhirui     add op function for device
 */

// @formatter:off
#include <dreamos.h>

static os_list_node_t device_list;//系统设备列表
static os_mutex_t os_device_global_lock;//设备管理器全局锁
static os_bool_t os_device_initialized = OS_FALSE;//设备管理器是否已初始化完成

/*!
 * 锁定设备管理器
 */
static void os_device_lock()
{
    os_mutex_lock(&os_device_global_lock);
}

/*!
 * 解锁设备管理器
 */
static void os_device_unlock()
{
    os_mutex_unlock(&os_device_global_lock);
}

/*!
 * 获取设备列表
 * @return 设备列表指针
 */
os_list_node_p os_device_get_list()
{
    return &device_list;
}

/*!
 * 从设备列表中寻找一个设备
 * @param name 设备名
 * @return 若找到，则返回设备结构体指针，否则返回OS_NULL
 */
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

/*!
 * 设备注册
 * @param dev 设备结构体指针
 * @return 成功返回OS_ERR_OK，若指定设备名已存在，返回-OS_ERR_EINVAL
 */
os_err_t os_device_register(os_device_p dev)
{
    OS_ANNOTATION_NEED_DEVICE();

    OS_ERR_RETURN_ERROR(os_device_find(dev -> name) != OS_NULL,-OS_ERR_EINVAL);

    os_device_lock();
    dev -> initialized = OS_TRUE;
    os_mutex_init(&dev -> lock);
    os_list_insert_tail(device_list,&dev -> node);
    os_device_unlock();
    return OS_ERR_OK;
}

/*!
 * 调用设备的初始化函数
 * @param dev 设备结构体指针
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_device_op_init(os_device_p dev)
{
    OS_ANNOTATION_NEED_DEVICE();
    //为了对指定设备上锁，需要先将设备管理器临时全局锁定
    os_device_lock();
    //这里未来应当先检测dev是否存在于设备列表中，以防止在试图上锁时，该设备已经被解注册（目前系统尚不支持设备解注册功能）
    os_mutex_lock(&dev -> lock);
    os_device_unlock();

    //设备只需初始化一次
    if(dev -> initialized)
    {
        os_mutex_unlock(&dev -> lock);
        return OS_ERR_OK;
    }

    if(dev -> ops -> init != OS_NULL)
    {
        os_err_t ret;

        if((ret = dev -> ops -> init(dev)) != OS_ERR_OK)
        {
            os_mutex_unlock(&dev -> lock);
            return ret;
        }
    }

    dev -> initialized = OS_TRUE;
    os_mutex_unlock(&dev -> lock);
    return OS_ERR_OK;
}

/*!
 * 调用设备的打开函数
 * @param dev 设备结构体指针
 * @param open_flag 打开标志，由OS_FILE_FLAG_开头的常量组成的位或序列组成
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_device_op_open(os_device_p dev,os_size_t open_flag)
{
    OS_ANNOTATION_NEED_DEVICE();
    //为了对指定设备上锁，需要先将设备管理器临时全局锁定
    os_device_lock();
    //这里未来应当先检测dev是否存在于设备列表中，以防止在试图上锁时，该设备已经被解注册（目前系统尚不支持设备解注册功能）
    os_mutex_lock(&dev -> lock);
    os_device_unlock();

    //打开设备之前先尝试初始化设备
    OS_ERR_GET_ERROR_AND_RETURN(os_device_op_init(dev));

    dev -> open_flag = open_flag;

    if(dev -> ops -> open != OS_NULL)
    {
        os_err_t ret;

        if((ret = dev -> ops -> open(dev,open_flag)) != OS_ERR_OK)
        {
            os_mutex_unlock(&dev -> lock);
            return ret;
        }
    }

    dev -> refcnt++;//增加设备引用数
    os_mutex_unlock(&dev -> lock);
    return OS_ERR_OK;
}

/*!
 * 调用设备的关闭函数
 * @param dev 设备结构体指针
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_device_op_close(os_device_p dev)
{
    OS_ANNOTATION_NEED_DEVICE();
    //为了对指定设备上锁，需要先将设备管理器临时全局锁定
    os_device_lock();
    os_mutex_lock(&dev -> lock);
    os_device_unlock();

    os_device_op_init(dev);

    if(dev -> ops -> close != OS_NULL)
    {
        os_err_t ret;

        if((ret = dev -> ops -> close(dev)) != OS_ERR_OK)
        {
            os_mutex_unlock(&dev -> lock);
            return ret;
        }
    }

    dev -> refcnt--;//减少设备引用数
    os_mutex_unlock(&dev -> lock);
    return OS_ERR_OK;
}

/*!
 * 调用设备的读取函数
 * @param dev 设备结构体指针
 * @param buf 数据缓冲区
 * @param pos 开始位置
 * @param size 数据量（字节数）
 * @return 成功返回读取的数据量（非负数），失败返回负数错误码
 */
os_err_t os_device_op_read(os_device_p dev,void *buf,os_size_t pos,os_size_t size)
{
    OS_ANNOTATION_NEED_DEVICE();
    os_mutex_lock(&dev -> lock);

    //权限检查
    if(dev -> open_flag & OS_FILE_FLAG_WRONLY)
    {
        os_mutex_unlock(&dev -> lock);
        return -OS_ERR_EACCES;
    }

    os_err_t ret = 0;

    if(dev -> ops -> read != OS_NULL)
    {
        ret = dev -> ops -> read(dev,buf,pos,size);
    }

    os_mutex_unlock(&dev -> lock);
    return ret;
}

/*!
 * 调用设备的写入函数
 * @param dev 设备结构体指针
 * @param buf 数据缓冲区
 * @param pos 开始位置
 * @param size 数据量（字节数）
 * @return 成功返回读取的数据量（非负数），失败返回负数错误码
 */
os_err_t os_device_op_write(os_device_p dev,const void *buf,os_size_t pos,os_size_t size)
{
    OS_ANNOTATION_NEED_DEVICE();
    os_mutex_lock(&dev -> lock);

    //权限检查
    if(!(dev -> open_flag & (OS_FILE_FLAG_WRONLY | OS_FILE_FLAG_RDWR)))
    {
        os_mutex_unlock(&dev -> lock);
        return -OS_ERR_EACCES;
    }

    os_err_t ret = 0;

    if(dev -> ops -> write != OS_NULL)
    {
        ret = dev -> ops -> write(dev,buf,pos,size);
    }

    os_mutex_unlock(&dev -> lock);
    return ret;
}

/*!
 * 调用设备的ioctl函数
 * @param dev 设备结构体指针
 * @param cmd 命令
 * @param arg 参数
 * @return 成功返回非负数（通常为OS_ERR_OK），失败返回负数错误码
 */
os_err_t os_device_op_ioctl(os_device_p dev,os_size_t cmd,os_size_t arg)
{
    OS_ANNOTATION_NEED_DEVICE();
    os_mutex_lock(&dev -> lock);

    os_err_t ret = OS_ERR_OK;

    if(dev -> ops -> ioctl != OS_NULL)
    {
        ret = dev -> ops -> ioctl(dev,cmd,arg);
    }

    os_mutex_unlock(&dev -> lock);
    return ret;
}

/*!
 * 判断设备管理器是否已经初始化完成
 * @return
 */
os_bool_t os_device_is_initialized()
{
    return os_device_initialized;
}

/*!
 * 初始化设备管理器
 */
void os_device_init()
{
    os_list_init(device_list);
    os_mutex_init(&os_device_global_lock);
    os_device_initialized = OS_TRUE;
}