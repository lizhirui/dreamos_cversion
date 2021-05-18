/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __OS_DEBUG_H__
#define __OS_DEBUG_H__

    void os_assert_handler(const char *ex_string,const char *func,size_t line);

    #define OS_ASSERT(EX)                                                         \
    if (!(EX))                                                                    \
    {                                                                             \
        os_assert_handler(#EX, __FUNCTION__, __LINE__);                           \
    }

#endif