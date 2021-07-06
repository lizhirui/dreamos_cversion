/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __DREAMOS_H__
#define __DREAMOS_H__

    #include <stdlib.h>
    #include <stdint.h>
    #include <stdarg.h>
    #include <sys/types.h>

    #include <osconfig.h>
    #include <os_def.h>
    
    #include <os_io.h>
    #include <os_tick.h>
    #include <bsp_interface.h>
    #include <os_string.h>
    #include <os_bitmap.h>
    #include <os_memory.h>
    #include <os_debug.h>
    #include <os_terminal_color.h>
    #include <os_annotation.h>
    #include <os_mmu.h>
    #include <os_list.h>
    #include <os_bitmap.h>
    #include <os_task.h>
    #include <os_interrupt.h>
    #include <os_waitqueue.h>
    #include <os_mutex.h>
    #include <os_device.h>
    #include <os_vfs.h>
    #include <os_syscall.h>

    #include <firmware.h>
    #include <arch.h>
    #include <bsp.h>

    void os_init();

#endif