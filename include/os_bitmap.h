/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-04     lizhirui     the first version
 */

// @formatter:off
#ifndef __OS_BITMAP_H__
#define __OS_BITMAP_H__

    #include <dreamos.h>

    //位图结构体指针
    typedef struct os_bitmap
    {
        os_size_t *memory;//位图内存
        os_size_t capacity;//位图内存实际大小（位数）
        os_size_t size;//位图内存计划大小（位数）
        os_bool_t allocated;//位图内存是否为自动分配
    }os_bitmap_t,*os_bitmap_p;

    os_err_t os_bitmap_create(os_bitmap_p bitmap,os_size_t size,void *memory,os_size_t default_value);
    void os_bitmap_remove(os_bitmap_p bitmap);
    os_size_t os_bitmap_get_bit(os_bitmap_p bitmap,os_size_t id);
    void os_bitmap_set_bit(os_bitmap_p bitmap,os_size_t id,os_size_t value);
    os_size_t os_bitmap_find_some_ones(os_bitmap_p bitmap,os_size_t start_id,os_size_t count);
    os_size_t os_bitmap_find_some_zeros(os_bitmap_p bitmap,os_size_t start_id,os_size_t count);
    void os_bitmap_set_bits(os_bitmap_p bitmap,os_size_t start_id,os_size_t count,os_size_t value);

#endif