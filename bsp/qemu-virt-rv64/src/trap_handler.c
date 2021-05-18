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

bool_t bsp_interrupt_handler(enum interrupt_type interrupt_type)
{
    if(interrupt_type == INTERRUPT_SUPERVISOR_TIMER)
    {
        os_enter_interrupt();
        tick_isr();
        os_leave_interrupt();
        return TRUE;
    }

    return FALSE;
}