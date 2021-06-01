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

static size_t interrupt_nest = 0;

bool_t bsp_interrupt_disable();
void bsp_interrupt_enable(bool_t enabled);

//进入中断时通知内核更新当前的中断层次
void os_enter_interrupt()
{
    interrupt_nest++;
}

//离开中断时通知内核更新当前的中断层次
void os_leave_interrupt()
{
    interrupt_nest--;
}

//判断当前程序是否运行在中断中
bool_t is_in_interrupt()
{
    return interrupt_nest > 0;
}

//关中断，并返回关中断前的中断状态
bool_t os_interrupt_disable()
{
    return bsp_interrupt_disable();
}

//开中断，或者说是还原中断状态
void os_interrupt_enable(bool_t enabled)
{
    bsp_interrupt_enable(enabled);
}