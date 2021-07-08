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

static os_size_t os_hashmap_default_hash_function(os_hashmap_p hashmap,os_size_t key)
{
    return key & MASK(hashmap -> count_bit);
}

os_err_t os_hashmap_create(os_hashmap_p hashmap,os_size_t count,os_hashmap_hash_function_t hash_function)
{
    OS_ANNOTATION_NEED_DYNAMIC_MEMORY();

    if(count == 0)
    {
        return -OS_ERR_EINVAL;
    }

    if(hash_function == OS_NULL)
    {
        hash_function = &os_hashmap_default_hash_function;
    }

    hashmap -> count_bit = ALIGN_UP_MIN(count);
    hashmap -> count = 1 << hashmap -> count_bit;
    hashmap -> list = os_memory_alloc(hashmap -> count * sizeof(os_list_node_t));
    OS_ERR_RETURN_ERROR(hashmap -> list == OS_NULL,-OS_ERR_ENOMEM);
    hashmap -> hash_function = hash_function;

    os_size_t i;

    for(i = 0;i < hashmap -> count;i++)
    {
        os_list_init(hashmap -> list[i]);
    }

    return OS_ERR_OK;
}

void os_hashmap_remove(os_hashmap_p hashmap)
{
    OS_ASSERT(hashmap -> list != OS_NULL);

    os_size_t i;

    for(i = 0;i < hashmap -> count;i++)
    {
        os_list_entry_foreach_safe(hashmap -> list[i],os_hashmap_item_t,node,entry,
        {
            os_list_node_remove(&entry -> node);
            os_memory_free(entry);
        });
    }

    os_memory_free(hashmap -> list);
    os_memset(hashmap,0,sizeof(*hashmap));
}

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

os_bool_t os_hashmap_get(os_hashmap_p hashmap,os_size_t key,void **value)
{
    os_size_t hash_id = hashmap -> hash_function(hashmap,key);
    OS_ASSERT(hash_id < hashmap -> count);
    os_hashmap_item_p item = __os_hashmap_find(hashmap,key,hash_id);

    if(value != OS_NULL)
    {
        *value = (item == OS_NULL) ? OS_NULL : item -> value;
    }

    return item != OS_NULL;
}

void os_hashmap_remove_item(os_hashmap_p hashmap,os_size_t key)
{
    os_size_t hash_id = hashmap -> hash_function(hashmap,key);
    OS_ASSERT(hash_id < hashmap -> count);
    os_hashmap_item_p item = __os_hashmap_find(hashmap,key,hash_id);

    if(item != OS_NULL)
    {
        os_list_node_remove(&item -> node);
        os_memory_free(item);
    }
}