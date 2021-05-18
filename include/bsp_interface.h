/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __BSP_INTERFACE_H__
#define __BSP_INTERRUPT_H__

    void bsp_early_init();
    void bsp_after_heap_init();
    void bsp_after_task_scheduler_init();

#endif