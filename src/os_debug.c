/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#include <dreamos.h>

void os_assert_handler(const char *ex_string,const char *func,size_t line)
{
    os_printf("(%s) assertion failed at function:%s, line number:%d \n",ex_string,func,line);
    while(1);
}