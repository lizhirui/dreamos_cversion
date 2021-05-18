/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

    #include <dreamos.h>
    
    void os_enter_interrupt();
    void os_leave_interrupt();
    bool_t is_in_interrupt();
    bool_t os_interrupt_disable();
    void os_interrupt_enable(bool_t enabled);
    
#endif