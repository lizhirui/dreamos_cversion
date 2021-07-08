/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-04     lizhirui     the first version
 * 2021-07-05     lizhirui     fix a bug of find some ones and zeros
 * 2021-07-08     lizhirui     modified the result value type of os_bitmap_create and fix a bug of os_bitmap_create memset size
 */

// @formatter:off
#include <dreamos.h>

#define GROUP_BITS OS_SIZE_T_BITS
#define GROUP_SIZE SIZE(GROUP_BITS)
#define GROUP_ID(x) ((x) >> GROUP_BITS)
#define INDEX_ID(x) MASK_VALUE(x,MASK(GROUP_BITS))

os_err_t os_bitmap_create(os_bitmap_p bitmap,os_size_t size,void *memory,os_size_t default_value)
{
    OS_ASSERT(bitmap != OS_NULL);
    OS_ASSERT(size != 0);
    OS_ASSERT(size != OS_NUMBER_MAX(os_size_t));

    bitmap -> capacity = ALIGN_UP(size,sizeof(os_size_t) << 3);
    bitmap -> size = size;
    bitmap -> allocated = memory == OS_NULL;

    if(bitmap -> allocated)
    {
        bitmap -> memory = os_memory_alloc(bitmap -> capacity / (sizeof(os_size_t) << 3));
        OS_ERR_RETURN_ERROR(bitmap -> memory == OS_NULL,-OS_ERR_ENOMEM);
    }
    else
    {
        bitmap -> memory = memory;
    }

    os_memset((void *)bitmap -> memory,default_value ? 0xFF : 0,bitmap -> capacity >> 3);
    return OS_ERR_OK;
}

void os_bitmap_remove(os_bitmap_p bitmap)
{
    if(bitmap -> allocated)
    {
        os_memory_free(bitmap -> memory);
        bitmap -> allocated = OS_FALSE;
    }

    bitmap -> size = 0;
    bitmap -> capacity = 0;
}

os_size_t os_bitmap_get_bit(os_bitmap_p bitmap,os_size_t id)
{
    OS_ASSERT(id < bitmap -> size);

    os_size_t group_id = GROUP_ID(id);
    os_size_t index_id = INDEX_ID(id);

    return (bitmap -> memory[group_id] >> index_id) & 0x01;
}

void os_bitmap_set_bit(os_bitmap_p bitmap,os_size_t id,os_size_t value)
{
    OS_ASSERT(id < bitmap -> size);

    os_size_t group_id = GROUP_ID(id);
    os_size_t index_id = INDEX_ID(id);

    bitmap -> memory[group_id] &= ~(1UL << index_id);
    bitmap -> memory[group_id] |= ((os_size_t)(value & 0x01)) << index_id;
}

os_size_t os_bitmap_find_some_ones(os_bitmap_p bitmap,os_size_t start_id,os_size_t count)
{
    os_size_t remainedcount = count;
    os_size_t ret = start_id;
    os_size_t i;

    for(i = start_id;i < bitmap -> size;)
    {
        os_size_t group_id = GROUP_ID(i);
        os_size_t index_id = INDEX_ID(i);

        if((index_id == 0) && (remainedcount >= GROUP_SIZE))
        {
            if(bitmap -> memory[group_id] == OS_NUMBER_MAX(os_size_t))
            {
                i += GROUP_SIZE;
                remainedcount -= GROUP_SIZE;

                if(remainedcount == 0)
                {
                    return ret;
                }

                continue;
            }
        }

        if((bitmap -> memory[group_id] >> index_id) & 0x01)
        {
            remainedcount--;

            if(remainedcount == 0)
            {
                return ret;
            }
        }
        else
        {
            remainedcount = count;
            ret = i + 1;
        }

        i++;
    }

    return OS_NUMBER_MAX(os_size_t);
}

os_size_t os_bitmap_find_some_zeros(os_bitmap_p bitmap,os_size_t start_id,os_size_t count)
{
    os_size_t remainedcount = count;
    os_size_t ret = start_id;
    os_size_t i;

    for(i = start_id;i < bitmap -> size;)
    {
        os_size_t group_id = GROUP_ID(i);
        os_size_t index_id = INDEX_ID(i);

        if((index_id == 0) && (remainedcount >= GROUP_SIZE))
        {
            if(bitmap -> memory[group_id] == 0)
            {
                i += GROUP_SIZE;
                remainedcount -= GROUP_SIZE;

                if(remainedcount == 0)
                {
                    return ret;
                }

                continue;
            }
        }

        if(!((bitmap -> memory[group_id] >> index_id) & 0x01))
        {
            remainedcount--;

            if(remainedcount == 0)
            {
                return ret;
            }
        }
        else
        {
            remainedcount = count;
            ret = i + 1;
        }

        i++;
    }

    return OS_NUMBER_MAX(os_size_t);
}

void os_bitmap_set_bits(os_bitmap_p bitmap,os_size_t start_id,os_size_t count,os_size_t value)
{
    os_size_t remainedcount = count;
    os_size_t i;

    OS_ASSERT((start_id + count) <= bitmap -> size);

    for(i = start_id;i < bitmap -> size;)
    {
        os_size_t group_id = GROUP_ID(i);
        os_size_t index_id = INDEX_ID(i);

        if((index_id == 0) && (remainedcount >= GROUP_SIZE))
        {
            bitmap -> memory[group_id] = value ? OS_NUMBER_MAX(os_size_t) : 0;
            i += GROUP_SIZE;
            remainedcount -= GROUP_SIZE;
        }
        else
        {
            if(value)
            {
                bitmap -> memory[group_id] |= 1UL << index_id;
            }
            else
            {
                bitmap -> memory[group_id] &= ~(1UL << index_id);
            }

            i++;
            remainedcount--;
        }

        if(remainedcount == 0)
        {
            break;
        }
    }
}
