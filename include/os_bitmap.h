/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-04     lizhirui     the first version
 */

#ifndef __OS_BITMAP_H__
#define __OS_BITMAP_H__

    #include <dreamos.h>

    typedef struct os_bitmap
    {
        os_size_t *memory;
        os_size_t capacity;
        os_size_t size;
        os_bool_t allocated;
    }os_bitmap_t,*os_bitmap_p;

    void os_bitmap_create(os_bitmap_p bitmap,os_size_t size,void *memory,os_size_t default_value);
    void os_bitmap_remove(os_bitmap_p bitmap);
    os_size_t os_bitmap_get_bit(os_bitmap_p bitmap,os_size_t id);
    void os_bitmap_set_bit(os_bitmap_p bitmap,os_size_t id,os_size_t value);
    os_size_t os_bitmap_find_some_ones(os_bitmap_p bitmap,os_size_t start_id,os_size_t count);
    os_size_t os_bitmap_find_some_zeros(os_bitmap_p bitmap,os_size_t start_id,os_size_t count);
    void os_bitmap_set_bits(os_bitmap_p bitmap,os_size_t start_id,os_size_t count,os_size_t value);

#endif