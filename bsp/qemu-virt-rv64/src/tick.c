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

static volatile uint64_t time_elapsed = 0;
static volatile uint64_t tick_cycles = 0;

uint64_t get_ticks()
{
    uint64_t r;

    asm volatile("rdtime %0" : "=r"(r));
    return r;
}

void tick_init()
{
    uint64_t interval = 10000ULL / TICK_PER_SECOND;
    clear_csr(sie,SIP_STIP);
    tick_cycles = 3686400 / 5000 * interval - 1;
    sbi_set_timer(get_ticks() + tick_cycles);
    set_csr(sie,SIP_STIP);
}

void tick_isr()
{
    kernel_tick();
    sbi_set_timer(get_ticks() + tick_cycles);
}