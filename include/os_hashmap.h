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
#ifndef __OS_HASHMAP_H__
#define __OS_HASHMAP_H__

    #include <dreamos.h>

    typedef struct os_hashmap os_hashmap_t,*os_hashmap_p;

    typedef os_size_t (*os_hashmap_hash_function_t)(os_hashmap_p hashmap,os_size_t key);

    //hashmap结构体
    struct os_hashmap
    {
        os_list_node_t *list;//哈希节点列表集合
        os_size_t count;//哈希节点列表数
        os_size_t count_bit;//哈希节点列表数的2的对数
        os_hashmap_hash_function_t hash_function;//哈希函数
    };

    //哈希节点
    typedef struct os_hashmap_item
    {
        os_size_t key;//键
        void *value;//值
        os_list_node_t node;//列表节点
    }os_hashmap_item_t,*os_hashmap_item_p;

    os_err_t os_hashmap_create(os_hashmap_p hashmap,os_size_t count,os_hashmap_hash_function_t hash_function);
    void os_hashmap_remove(os_hashmap_p hashmap);
    os_err_t os_hashmap_set(os_hashmap_p hashmap,os_size_t key,void *value);
    os_bool_t os_hashmap_get(os_hashmap_p hashmap,os_size_t key,void **value);
    void os_hashmap_remove_item(os_hashmap_p hashmap,os_size_t key);

#endif
