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

//为了让位图能够充分利用处理器架构位数的优势，这里规定一个group的大小就是处理器的最大可处理字长（即sizeof(os_size_t)）
//group部分起始位
#define GROUP_BITS OS_SIZE_T_BITS
//一个group能存储的位
#define GROUP_SIZE SIZE(GROUP_BITS)
//获取一个位地址的group部分
#define GROUP_ID(x) ((x) >> GROUP_BITS)
//获取一个位地址的index部分
#define INDEX_ID(x) MASK_VALUE(x,MASK(GROUP_BITS))

/*!
 * 创建一个位图
 * @param bitmap 位图结构体指针
 * @param size 位图的大小（即位图的位数量），不可为0，不可为OS_NUMBER_MAX(os_size_t)
 * @param memory 位图数据内存地址，若为OS_NULL，则由系统自动分配
 * @param default_value 位图每一位的默认值，该值为0，则表示每一位都为0，否则每一位都为1
 * @return 若成功，则返回OS_ERR_OK，若位图内存自动分配失败，则返回-OS_ERR_ENOMEM
 */
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

/*!
 * 销毁一个位图，该位图的内存若为自动分配，则会自动销毁，否则需要由用户自行处理
 * @param bitmap 要销毁的位图结构体指针
 */
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

/*!
 * 获取位图的指定位
 * @param bitmap 位图结构体指针
 * @param id 位号（从0开始）
 * @return 该位的值（0或1）
 */
os_size_t os_bitmap_get_bit(os_bitmap_p bitmap,os_size_t id)
{
    OS_ASSERT(id < bitmap -> size);

    os_size_t group_id = GROUP_ID(id);
    os_size_t index_id = INDEX_ID(id);

    return (bitmap -> memory[group_id] >> index_id) & 0x01;
}

/*!
 * 设置位图的指定位
 * @param bitmap 位图结构体指针
 * @param id 位号（从0开始）
 * @param value 该位的值（0或1）
 */
void os_bitmap_set_bit(os_bitmap_p bitmap,os_size_t id,os_size_t value)
{
    OS_ASSERT(id < bitmap -> size);

    os_size_t group_id = GROUP_ID(id);
    os_size_t index_id = INDEX_ID(id);

    bitmap -> memory[group_id] &= ~(1UL << index_id);
    bitmap -> memory[group_id] |= ((os_size_t)(value & 0x01)) << index_id;
}

/*!
 * 找到位图中连续的若干个1，并返回起始位号（从0开始）
 * @param bitmap 位图结构体指针
 * @param start_id 开始查找的位号
 * @param count 要求的连续1数量
 * @return 起始位号，失败返回OS_NUMBER_MAX(os_size_t)
 */
os_size_t os_bitmap_find_some_ones(os_bitmap_p bitmap,os_size_t start_id,os_size_t count)
{
    os_size_t remainedcount = count;
    os_size_t ret = start_id;
    os_size_t i;

    for(i = start_id;i < bitmap -> size;)
    {
        os_size_t group_id = GROUP_ID(i);
        os_size_t index_id = INDEX_ID(i);

        //若找到一个空闲组，则直接记入，以优化查找速度
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

        //逐位判断
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

/*!
 * 找到位图中连续的若干个0，并返回起始位号（从0开始）
 * @param bitmap 位图结构体指针
 * @param start_id 开始查找的位号
 * @param count 要求的连续0数量
 * @return 起始位号，失败返回OS_NUMBER_MAX(os_size_t)
 */
os_size_t os_bitmap_find_some_zeros(os_bitmap_p bitmap,os_size_t start_id,os_size_t count)
{
    os_size_t remainedcount = count;
    os_size_t ret = start_id;
    os_size_t i;

    for(i = start_id;i < bitmap -> size;)
    {
        os_size_t group_id = GROUP_ID(i);
        os_size_t index_id = INDEX_ID(i);

        //若找到一个空闲组，则直接记入，以优化查找速度
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

        //逐位判断
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

/*!
 * 批量设置位图的某些位为指定值
 * @param bitmap 位图结构体指针
 * @param start_id 开始位号（从0开始）
 * @param count 要设置的位数量
 * @param value 值（0或1）
 */
void os_bitmap_set_bits(os_bitmap_p bitmap,os_size_t start_id,os_size_t count,os_size_t value)
{
    os_size_t remainedcount = count;
    os_size_t i;

    OS_ASSERT((start_id + count) <= bitmap -> size);

    for(i = start_id;i < bitmap -> size;)
    {
        os_size_t group_id = GROUP_ID(i);
        os_size_t index_id = INDEX_ID(i);

        //成组赋值，以优化赋值速度
        if((index_id == 0) && (remainedcount >= GROUP_SIZE))
        {
            bitmap -> memory[group_id] = value ? OS_NUMBER_MAX(os_size_t) : 0;
            i += GROUP_SIZE;
            remainedcount -= GROUP_SIZE;
        }
        else
        {
            //逐位赋值
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
