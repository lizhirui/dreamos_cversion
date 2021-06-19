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
    
    #include <io.h>
    #include <bsp_interface.h>
    #include <os_string.h>
    #include <os_memory.h>
    #include <os_debug.h>
    #include <os_terminal_color.h>
    #include <os_annotation.h>

    #include <list.h>
    #include <task.h>
    #include <interrupt.h>
    #include <device.h>
    #include <fs.h>

    #include <firmware.h>
    #include <arch.h>
    #include <bsp.h>

    void kernel_init();
    void kernel_tick();

#endif