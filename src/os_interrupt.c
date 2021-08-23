/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

// @formatter:off
#include <dreamos.h>

//中断嵌套层次，若为0，则表示当前不在中断上下文中
static os_size_t interrupt_nest = 0;

os_bool_t bsp_interrupt_disable();
void bsp_interrupt_enable(os_bool_t enabled);

/*!
 * 进入中断时通知内核更新当前的中断层次
 */
void os_enter_interrupt()
{
    interrupt_nest++;
}

/*!
 * 离开中断时通知内核更新当前的中断层次
 */
void os_leave_interrupt()
{
    interrupt_nest--;
}

/*!
 * 判断当前程序是否运行在中断中
 * @return 在中断中返回OS_TRUE，否则返回OS_FALSE
 */
os_bool_t os_is_in_interrupt()
{
    return interrupt_nest > 0;
}

/*!
 * 关中断，并返回关中断前的中断状态
 * @return 之前的中断状态
 */
os_bool_t os_interrupt_disable()
{
    return bsp_interrupt_disable();
}

/*!
 * 开中断，或者说是还原中断状态
 * @param enabled 要还原的目标中断状态
 */
void os_interrupt_enable(os_bool_t enabled)
{
    bsp_interrupt_enable(enabled);
}