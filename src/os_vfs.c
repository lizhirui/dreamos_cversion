/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-05     lizhirui     the first version
 * 2021-07-06     lizhirui     add finer-grained lock
 */

// @formatter:off
#include <dreamos.h>

static os_list_node_t fs_list;//文件系统列表
static os_list_node_t mount_list;//文件系统挂载表

static os_mutex_t vfs_global_lock;//VFS全局锁

//标识VFS是否初始化完成
static os_bool_t os_vfs_initialized = OS_FALSE;

/*!
 * VFS加锁
 */
void vfs_lock()
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_lock(&vfs_global_lock);
}

/*!
 * VFS解锁
 */
void vfs_unlock()
{
    OS_ANNOTATION_NEED_VFS();
    os_mutex_unlock(&vfs_global_lock);
}

/*!
 * 通过文件系统名称在文件系统列表中文件系统结构体指针
 * @param fs_name 文件系统名称
 * @return 若找到，返回文件系统结构体指针，否则返回OS_NULL
 */
static os_vfs_p os_vfs_find_fs_by_name(const char *fs_name)
{
    OS_ANNOTATION_NEED_VFS();
    vfs_lock();

    os_list_entry_foreach(fs_list,os_vfs_t,node,entry,
    {
        if(os_strcmp(entry -> name,fs_name) == 0)
        {
            vfs_unlock();
            return entry;
        }
    });

    vfs_unlock();
    return OS_NULL;
}

/*!
 * 注册新的文件系统
 * @param fs 文件系统结构体指针
 * @return 成功返回OS_ERR_OK，若文件系统名称已存在，则返回-OS_ERR_EINVAL
 */
os_err_t os_vfs_register(const os_vfs_p fs)
{
    OS_ANNOTATION_NEED_VFS();

    if(os_vfs_find_fs_by_name(fs -> name) != OS_NULL)
    {
        return -OS_ERR_EINVAL;
    }

    vfs_lock();
    os_mutex_init(&fs -> lock);
    fs -> mount_refcnt = 0;
    os_list_insert_tail(fs_list,&fs -> node);
    vfs_unlock();
    return OS_ERR_OK;
}

/*!
 * 正规化路径，该过程会去除路径中所有的.和..，并保证不出现连续的两个/，同时保证结尾不为/除非为根目录
 * @param path 待正规化的路径
 * @param buf 用于正规化路径保存的缓冲区
 * @return 成功返回OS_ERR_OK，若路径长度超过OS_VFS_PATH_MAX或路径非法，返回-OS_ERR_EINVAL
 */
os_err_t os_vfs_normalize_path(const char *path,char *buf)
{
    OS_ASSERT(path != OS_NULL);
    os_size_t len = os_strlen(path);

    if(len == 0)
    {
        buf[0] = '\0';
        return -OS_ERR_EINVAL;
    }

    OS_ERR_RETURN_ERROR(len > OS_VFS_PATH_MAX,-OS_ERR_EINVAL);

    //路径必须以斜杠开始
    if(path[0] != '/')
    {
        buf[0] = '\0';
        return -OS_ERR_EINVAL;
    }

    os_size_t i,j;

    for(i = 0,j = 0;i < len;)
    {
        if(i > 0)
        {
            //排除双斜杠的非法情况
            if((path[i - 1] == '/') && (path[i] == '/'))
            {
                buf[0] = '\0';
                return -OS_ERR_EINVAL;
            }
        }

        //点一定不会出现在第一个字符
        if((path[i] == '.') && (path[i - 1] == '/'))
        {
            if((path[i + 1] == '\0') || (path[i + 1] == '/'))
            {
                //当前目录
                i += 2;
                continue;
            }
            else if((path[i + 1] == '.') && ((path[i + 2] == '\0') || (path[i + 2] == '/')))
            {
                //父目录
                i += 3;

                //缓冲区一定以斜杠作为结尾，但这里要跳过根目录的情况，缓冲区第一个字符一定是斜杠
                if(j > 1)
                {
                    j--;

                    while(buf[j - 1] != '/')
                    {
                        j--;
                    }
                }

                continue;
            }
        }

        //不允许出现反斜杠
        if(path[i] == '\\')
        {
            buf[0] = '\0';
            return -OS_ERR_EINVAL;
        }

        //普通字符复制
        buf[j++] = path[i++];
    }

    //除非是根目录，否则去掉结尾的斜杠
    if(j > 1)
    {
        while(buf[j - 1] == '/')
        {
            j--;
        }
    }

    //终结字符串
    buf[j] = '\0';
    return OS_ERR_OK;
}

/*!
 * 根据路径查找挂载点，并返回挂载点结构体指针，原理是进行前缀路径匹配，最长的前缀匹配的挂载点就是目标挂载点
 * @param path 路径
 * @return 若找到则返回挂载点结构体指针，否则返回OS_NULL
 */
os_vfs_mp_p os_vfs_find_mp_by_path(const char *path)
{
    OS_ANNOTATION_NEED_VFS();
    vfs_lock();
    
    OS_ASSERT(path != OS_NULL);

    os_size_t path_len = os_strlen(path);

    os_vfs_mp_p ret = OS_NULL;
    os_size_t match_len = 0;

    os_list_entry_foreach(mount_list,os_vfs_mp_t,node,entry,
    {
        os_size_t len = os_strlen(entry -> path);

        if((len > match_len) && (len <= path_len))
        {
            if(os_memcmp(entry -> path,path,len) == 0)
            {
                ret = entry;
                match_len = len;
            }
        }
    });

    vfs_unlock();
    return ret;
}

/*!
 * 挂载文件系统
 * @param mount_path 挂载路径
 * @param fs_name 文件系统名称
 * @param dev 设备名，可为OS_NULL
 * @param mount_flag 挂载标志，参考OS_FILE_FLAG_开头的标志
 * @param priv_data 私有数据
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_vfs_mount(const char *mount_path,const char *fs_name,const char *dev,os_size_t mount_flag,void *priv_data)
{
    OS_ANNOTATION_NEED_VFS();
    vfs_lock();

    os_err_t ret = OS_ERR_OK;
    //检测文件系统是否存在
    os_vfs_p fs = os_vfs_find_fs_by_name(fs_name);

    if(fs == OS_NULL)
    {
        vfs_unlock();
        return -OS_ERR_EINVAL;
    }

    //对文件系统加锁
    os_mutex_lock(&fs -> lock);
    //对路径进行正规化
    char *path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,path_buf_alloc_err);
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(mount_path,path_buf),ret,err);
    os_vfs_mp_p mount_mp = OS_NULL;

    //特殊处理根文件系统的挂载，对于非根文件系统，这里获取挂载点所在的文件系统的挂载点
    if(os_strcmp(mount_path,"/") != 0)
    {
        os_file_state_t state;
        mount_mp = os_vfs_find_mp_by_path(path_buf);
        OS_ANNOTATION(mount_mp != OS_NULL,"vfs_mount needs root filesystem!");
        OS_ERR_GET_ERROR_AND_GOTO(mount_mp -> fs -> ops -> stat(mount_mp,path_buf,&state),ret,err);
        OS_ERR_SET_ERROR_AND_GOTO(state.type != OS_FILE_TYPE_DIRECTORY,ret,-OS_ERR_ENOTDIR,err);
    }

    //若设备名不为OS_NULL，则获取对应的设备结构体指针
    os_device_p dev_obj = OS_NULL;

    if(dev != OS_NULL)
    {
        dev_obj = os_device_find(dev);
        OS_ERR_SET_ERROR_AND_GOTO(dev_obj == OS_NULL,ret,-OS_ERR_EINVAL,err);
    }

    //分配新的挂载点
    os_vfs_mp_p mp = os_memory_alloc(sizeof(os_vfs_mp_t));
    OS_ERR_SET_ERROR_AND_GOTO(mp == OS_NULL,ret,-OS_ERR_ENOMEM,err);

    mp -> fs = fs;
    os_strcpy(mp -> path,path_buf);
    //计算文件系统子路径偏移
    mp -> subpath_offset = os_strlen(mp -> path);

    //若为根文件系统，则不需要子路径偏移，否则需要+1跳过/
    if(mp -> subpath_offset == 1)
    {
        mp -> subpath_offset = 0;
    }
    else
    {
        mp -> subpath_offset++;
    }

    mp -> dev_path = dev;
    mp -> dev = dev_obj;

    mp -> mount_flag = mount_flag;
    os_mutex_init(&mp -> lock);
    mp -> mp_cnt = 0;
    mp -> open_file_cnt = 0;
    ret = fs -> ops -> mount(mp,mount_flag,priv_data);

    if(ret != OS_ERR_OK)
    {
        os_memory_free(mp);
        goto err;
    }

    //增加父挂载点的引用数
    if(mount_mp != OS_NULL)
    {
        mount_mp -> mp_cnt++;
    }

    os_list_insert_tail(mount_list,&mp -> node);
    //增加挂载点对应文件系统的应用数
    fs -> mount_refcnt++;

err:
    os_memory_free(path_buf);
path_buf_alloc_err:
    os_mutex_unlock(&fs -> lock);
    vfs_unlock();
    return ret;
}

/*!
 * 卸载文件系统
 * @param mount_path 挂载路径
 * @return 成功返回OS_TRUE，失败返回负数错误码
 */
os_err_t os_vfs_unmount(const char *mount_path)
{
    OS_ANNOTATION_NEED_VFS();
    vfs_lock();

    os_err_t ret = OS_ERR_OK;
    
    char *path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,path_buf_alloc_err);
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(mount_path,path_buf),ret,err);

    os_vfs_mp_p mp = os_vfs_find_mp_by_path(path_buf);
    OS_ERR_SET_ERROR_AND_GOTO(mp == OS_NULL,ret,-OS_ERR_EINVAL,err);
    OS_ERR_SET_ERROR_AND_GOTO(mp -> mp_cnt > 0,ret,-OS_ERR_EPERM,err);
    OS_ERR_SET_ERROR_AND_GOTO(mp -> open_file_cnt > 0,ret,-OS_ERR_EPERM,err);
    OS_ERR_GET_ERROR_AND_GOTO(mp -> fs -> ops -> unmount(mp),ret,err);
    os_list_node_remove(&mp -> node);
    os_mutex_lock(&mp -> fs -> lock);
    mp -> fs -> mount_refcnt--;
    os_mutex_unlock(&mp -> fs -> lock);
    os_memory_free(mp);

    os_vfs_mp_p mount_mp = os_vfs_find_mp_by_path(path_buf);
    OS_ASSERT(mount_mp != OS_NULL);
    mount_mp -> mp_cnt--;

err:
    os_memory_free(path_buf);
path_buf_alloc_err:
    vfs_unlock();
    return ret;
}

/*!
 * 格式化文件系统
 * @param fs_name 文件系统名
 * @param dev 设备名
 * @return
 */
os_err_t os_vfs_mkfs(const char *fs_name,const char *dev)
{
    OS_ANNOTATION_NOT_IMPLEMENT();
    OS_ANNOTATION_NEED_VFS();

    os_err_t ret = OS_ERR_OK;

    vfs_lock();
    os_vfs_p fs = os_vfs_find_fs_by_name(fs_name);
    OS_ERR_SET_ERROR_AND_GOTO(fs == OS_NULL,ret,-OS_ERR_EINVAL,err);
    OS_ERR_SET_ERROR_AND_GOTO(dev == OS_NULL,ret,-OS_ERR_EINVAL,err);
    os_device_p dev_obj = os_device_find(dev);
    OS_ERR_SET_ERROR_AND_GOTO(dev_obj == OS_NULL,ret,-OS_ERR_EINVAL,err);
    os_mutex_lock(&fs -> lock);
    vfs_unlock();
    ret = fs -> ops -> mkfs(fs,dev_obj);
    os_mutex_unlock(&fs -> lock);
    return ret;

err:
    vfs_unlock();
    return ret;
}

/*!
 * 获取文件系统状态信息
 * @param mount_path 文件系统挂载路径，可以为该挂载点内的子路径
 * @param state 返回的文件系统状态信息
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_vfs_statfs(const char *mount_path,os_vfs_state_p state)
{
    OS_ANNOTATION_NEED_VFS();
    vfs_lock();

    os_err_t ret = OS_ERR_OK;
    
    char *path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,path_buf_alloc_err);
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(mount_path,path_buf),ret,err);

    os_vfs_mp_p mp = os_vfs_find_mp_by_path(path_buf);
    OS_ERR_SET_ERROR_AND_GOTO(mp == OS_NULL,ret,-OS_ERR_EINVAL,err);
    os_mutex_lock(&mp -> lock);
    vfs_unlock();
    ret = mp -> fs -> ops -> statfs(mp,state);
    os_mutex_unlock(&mp -> lock);
    os_memory_free(path_buf);
    return ret;

err:
    os_memory_free(path_buf);
path_buf_alloc_err:
    vfs_unlock();
    return ret;
}

/*!
 * 删除文件
 * @param path 文件路径
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_vfs_unlink(const char *path)
{
    OS_ANNOTATION_NEED_VFS();
    vfs_lock();

    os_err_t ret = OS_ERR_OK;
    
    char *path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,path_buf_alloc_err);
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(path,path_buf),ret,err);

    os_vfs_mp_p mp = os_vfs_find_mp_by_path(path_buf);
    OS_ERR_SET_ERROR_AND_GOTO(mp == OS_NULL,ret,-OS_ERR_EINVAL,err);
    os_mutex_lock(&mp -> lock);
    vfs_unlock();
    ret = mp -> fs -> ops -> unlink(mp,path);
    os_mutex_unlock(&mp -> lock);
    os_memory_free(path_buf);
    return ret;

err:
    os_memory_free(path_buf);
path_buf_alloc_err:
    vfs_unlock();
    return ret;
}

/*!
 * 获取文件状态信息
 * @param path 文件路径
 * @param state 返回的文件状态信息
 * @return 成功返回OS_ERR_OK，失败返回负数错误码
 */
os_err_t os_vfs_stat(const char *path,os_file_state_p state)
{
    OS_ANNOTATION_NEED_VFS();
    vfs_lock();

    os_err_t ret = OS_ERR_OK;
    
    char *path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,path_buf_alloc_err);
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(path,path_buf),ret,err);

    os_vfs_mp_p mp = os_vfs_find_mp_by_path(path_buf);
    OS_ERR_SET_ERROR_AND_GOTO(mp == OS_NULL,ret,-OS_ERR_EINVAL,err);
    os_mutex_lock(&mp -> lock);
    vfs_unlock();
    ret = mp -> fs -> ops -> stat(mp,path,state);
    os_mutex_unlock(&mp -> lock);
    os_memory_free(path_buf);
    return ret;

err:
    os_memory_free(path_buf);
path_buf_alloc_err:
    vfs_unlock();
    return ret;
}

/*!
 * 重命名文件，不支持跨文件系统
 * @param old_path 文件旧路径
 * @param new_path 文件新路径
 * @return 成功返回OS_ERR_OK，失败返回负数错误码，返回-OS_ERR_EXDEV可能是因为两个路径不在一个文件系统内
 */
os_err_t os_vfs_rename(const char *old_path,const char *new_path)
{
    OS_ANNOTATION_NEED_VFS();
    vfs_lock();

    os_err_t ret = OS_ERR_OK;
    
    char *old_path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(old_path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,old_path_buf_alloc_err);
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(old_path,old_path_buf),ret,err);

    char *new_path_buf = os_memory_alloc(OS_VFS_PATH_MAX + 1);
    OS_ERR_SET_ERROR_AND_GOTO(old_path_buf == OS_NULL,ret,-OS_ERR_ENOMEM,new_path_buf_alloc_err);
    OS_ERR_GET_ERROR_AND_GOTO(os_vfs_normalize_path(new_path,new_path_buf),ret,err);

    os_vfs_mp_p old_mp = os_vfs_find_mp_by_path(old_path_buf);
    OS_ERR_SET_ERROR_AND_GOTO(old_mp == OS_NULL,ret,-OS_ERR_EINVAL,err);
    
    os_vfs_mp_p new_mp = os_vfs_find_mp_by_path(new_path_buf);
    OS_ERR_SET_ERROR_AND_GOTO(new_mp == OS_NULL,ret,-OS_ERR_EINVAL,err);

    OS_ERR_SET_ERROR_AND_GOTO(old_mp != new_mp,ret,-OS_ERR_EXDEV,err);

    os_mutex_lock(&old_mp -> lock);
    vfs_unlock();
    ret = old_mp -> fs -> ops -> rename(old_mp,old_path_buf,new_path_buf);
    os_mutex_unlock(&old_mp -> lock);
    os_memory_free(new_path_buf);
    os_memory_free(old_path_buf);
    return ret;

err:
    os_memory_free(new_path_buf);
new_path_buf_alloc_err:
    os_memory_free(old_path_buf);
old_path_buf_alloc_err:
    vfs_unlock();
    return ret;
}

/*!
 * 指示VFS是否完成初始化
 * @return 若完成初始化返回OS_TRUE，否则返回OS_FALSE
 */
os_bool_t os_vfs_is_initialized()
{
    return os_vfs_initialized;
}

/*!
 * VFS初始化函数
 */
void os_vfs_init()
{
    os_list_init(fs_list);
    os_list_init(mount_list);
    os_mutex_init(&vfs_global_lock);
    os_file_init();
    os_vfs_initialized = OS_TRUE;
}
