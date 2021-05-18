/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __TICK_H__
#define __TICK_H__

    uint64_t get_ticks();
    void tick_init();
    void tick_isr();

#endif