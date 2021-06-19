/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __OS_DEF_H__
#define __OS_DEF_H__

    #include <stddef.h>

    #define SIZE(bit) (1UL << (bit))
    #define MASK(bit) (SIZE(bit) - 1UL)
    #define UMASK(bit) (~(MASK(bit)))
    #define MASK_VALUE(value,maskvalue) ((value) & (maskvalue))
    #define UMASK_VALUE(value,maskvalue) ((value) & (~(maskvalue)))
    #define CHECK_UPBOUND(value,bit_count) (!(((size_t)value) & (~MASK(bit_count))))
    #define CHECK_ALIGN(value,start_bit) (!(((size_t)value) & (MASK(start_bit))))

    #define PARTBIT(value,start_bit,length) (((value) >> (start_bit)) & MASK(length))

    #define IS_POWER_OF_2(value) (((value) & ((value) - 1)) == 0)

    #define ALIGN_UP(value,align_bound) (((value) + ((align_bound) - 1)) & (~((align_bound) - 1)))
    #define ALIGN_DOWN(value,align_bound) ((value) & (~(align_bound - 1)))
    #define ALIGN_DOWN_MAX(value) ((sizeof(size_t) << 3) - __builtin_clzl(value) - 1)
    #define ALIGN_UP_MIN(value) (IS_POWER_OF_2(value) ? ALIGN_DOWN_MAX(value) : (ALIGN_DOWN_MAX(value) + 1))

    #define MAX(a,b) (((a) > (b)) ? (a) : (b))
    #define MIN(a,b) (((a) < (b)) ? (a) : (b))

    #define ADDR_OFFSET(addr,offset) ((void *)(((size_t)(addr)) + ((size_t)(offset))))

    typedef uint8_t os_uint8_t;
    typedef uint16_t os_uint16_t;
    typedef uint32_t os_uint32_t;
    typedef uint64_t os_uint64_t;
    typedef size_t os_size_t;

    typedef int8_t os_int8_t;
    typedef int16_t os_int16_t;
    typedef int32_t os_int32_t;
    typedef int64_t os_int64_t;
    typedef ssize_t os_ssize_t;

    typedef size_t os_bool_t;

    #define OS_TRUE 1
    #define OS_FALSE 0

    #define OS_NULL NULL

    #define OS_ENTER_CRITICAL_AREA() os_bool_t interrupt_state = os_interrupt_disable()
    #define OS_LEAVE_CRITICAL_AREA() os_interrupt_enable(interrupt_state)

#endif