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

    typedef size_t bool_t;

    #define TRUE 1
    #define FALSE 0

    #define ENTER_CRITICAL_AREA() bool_t interrupt_state = os_interrupt_disable()
    #define LEAVE_CRITICAL_AREA() os_interrupt_enable(interrupt_state)

#endif