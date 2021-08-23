/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-07     lizhirui     the first version
 */

// @formatter:off
#include <dreamos.h>

/*!
 * 默认哈希函数
 * @param hashmap hashmap结构体指针
 * @param key 键
 * @return
 */
static os_size_t os_hashmap_default_hash_function(os_hashmap_p hashmap,os_size_t key)
{
    return key & MASK(hashmap -> count_bit);
}

/*!
 * 创建hashmap
 * @param hashmap hashmap结构体指针
 * @param count hashmap大小，若不为2的幂，则会被扩增到大于该数的最小的2的幂数，该值仅和哈希性能相关，该值不可为0
 * @param hash_function 哈希函数，若为OS_NULL，则采用默认的哈希函数
 * @return 成功返回OS_ERR_OK，若count为0，则返回-OS_ERR_EINVAL，若哈希列表空间分配是啊比，则返回-OS_ERR_ENOMEM
 */
os_err_t os_hashmap_create(os_hashmap_p hashmap,os_size_t count,os_hashmap_hash_function_t hash_function)
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();

    if(count == 0)
    {
        return -OS_ERR_EINVAL;
    }

    //默认哈希函数
    if(hash_function == OS_NULL)
    {
        hash_function = &os_hashmap_default_hash_function;
    }

    //计算出哈希大小所占的位数并扩增到最小的2次幂
    hashmap -> count_bit = ALIGN_UP_MIN(count);
    hashmap -> count = 1 << hashmap -> count_bit;
    //分配哈希列表空间
    hashmap -> list = os_memory_alloc(hashmap -> count * sizeof(os_list_node_t));
    OS_ERR_RETURN_ERROR(hashmap -> list == OS_NULL,-OS_ERR_ENOMEM);
    hashmap -> hash_function = hash_function;

    os_size_t i;

    //初始化哈希列表
    for(i = 0;i < hashmap -> count;i++)
    {
        os_list_init(hashmap -> list[i]);
    }

    return OS_ERR_OK;
}

/*!
 * 销毁hashmap
 * @param hashmap hashmap结构体指针
 */
void os_hashmap_remove(os_hashmap_p hashmap)
{
    OS_ASSERT(hashmap -> list != OS_NULL);

    os_size_t i;

    //首先销毁哈希列表中的每个节点
    for(i = 0;i < hashmap -> count;i++)
    {
        os_list_entry_foreach_safe(hashmap -> list[i],os_hashmap_item_t,node,entry,
        {
            os_list_node_remove(&entry -> node);
            os_memory_free(entry);
        });
    }

    //然后销毁整个哈希列表
    os_memory_free(hashmap -> list);
    //最后对hashmap结构体清零
    os_memset(hashmap,0,sizeof(*hashmap));
}

/*!
 * 在hashmap中根据键和哈希id查找对应的哈希项结构体指针
 * @param hashmap hashmap结构体指针
 * @param key 键
 * @param hash_id 哈希id
 * @return 若找到则返回对于的哈希项结构体指针，否则返回OS_NULL
 */
static os_hashmap_item_p __os_hashmap_find(os_hashmap_p hashmap,os_size_t key,os_size_t hash_id)
{
    os_list_entry_foreach(hashmap -> list[hash_id],os_hashmap_item_t,node,entry,
    {
        if(entry -> key == key)
        {
            return entry;
        }
    });

    return OS_NULL;
}

/*!
 * 设置某个键对应的值，如果对应的键不存在，将会自动创建
 * @param hashmap hashmap结构体指针
 * @param key 键
 * @param value 值
 * @return 成功返回OS_ERR_OK，哈希项分配失败返回-OS_ERR_ENOMEM
 */
os_err_t os_hashmap_set(os_hashmap_p hashmap,os_size_t key,void *value)
{
    os_size_t hash_id = hashmap -> hash_function(hashmap,key);
    OS_ASSERT(hash_id < hashmap -> count);
    os_hashmap_item_p item = __os_hashmap_find(hashmap,key,hash_id);

    if(item == OS_NULL)
    {
        item = os_memory_alloc(sizeof(os_hashmap_item_t));
        OS_ERR_RETURN_ERROR(item == OS_NULL,-OS_ERR_ENOMEM);
        item -> key = key;
        os_list_insert_tail(hashmap -> list[hash_id],&item -> node);
    }

    item -> value = value;
    return OS_ERR_OK;
}

/*!
 * 获取某个键对应的值
 * @param hashmap hashmap结构体指针
 * @param key 键
 * @param value 返回的值
 * @return 成功返回OS_TRUE，失败返回OS_FALSE，并且会设置返回的值为OS_NULL
 */
os_bool_t os_hashmap_get(os_hashmap_p hashmap,os_size_t key,void **value)
{
    //调用哈希函数计算哈希id
    os_size_t hash_id = hashmap -> hash_function(hashmap,key);
    OS_ASSERT(hash_id < hashmap -> count);
    //执行查找操作
    os_hashmap_item_p item = __os_hashmap_find(hashmap,key,hash_id);

    if(value != OS_NULL)
    {
        *value = (item == OS_NULL) ? OS_NULL : item -> value;
    }

    return item != OS_NULL;
}

/*!
 * 删除某个键
 * @param hashmap hashmap结构体指针
 * @param key 键
 */
void os_hashmap_remove_item(os_hashmap_p hashmap,os_size_t key)
{
    //调用哈希函数计算哈希id
    os_size_t hash_id = hashmap -> hash_function(hashmap,key);
    OS_ASSERT(hash_id < hashmap -> count);
    //执行查找操作
    os_hashmap_item_p item = __os_hashmap_find(hashmap,key,hash_id);

    //若找到，则销毁对应项
    if(item != OS_NULL)
    {
        os_list_node_remove(&item -> node);
        os_memory_free(item);
    }
}