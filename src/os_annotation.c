/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-20     lizhirui     the first version
 */

// @formatter:off
#include <dreamos.h>

/*!
 * 注解违例处理程序
 * @param ex_string 出错代码
 * @param func 出错函数
 * @param line 出错行号
 * @param error_msg 错误消息
 */
void os_annotation_handler(const char *ex_string,const char *func,os_size_t line,const char *error_msg)
{
    terminal_color_set(TERMINAL_COLOR_RED,TERMINAL_COLOR_BLACK);
    os_printf("(%s) assertion failed at function:%s, line number:%d \n%s\n",ex_string,func,line,error_msg);
    terminal_color_set(TERMINAL_COLOR_WHITE,TERMINAL_COLOR_BLACK);
    asm volatile("ebreak");
}