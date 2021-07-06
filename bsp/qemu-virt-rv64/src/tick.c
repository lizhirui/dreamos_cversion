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

static volatile os_uint64_t time_elapsed = 0;
static volatile os_uint64_t tick_cycles = 0;

os_uint64_t get_ticks()
{
    os_uint64_t r;

    asm volatile("rdtime %0" : "=r"(r));
    return r;
}

void tick_init()
{
    os_uint64_t interval = 1000ULL / TICK_PER_SECOND;
    clear_csr(sie,SIP_STIP);
    tick_cycles = 10000000 / 1000 * interval - 1;
    sbi_set_timer(get_ticks() + tick_cycles);
    set_csr(sie,SIP_STIP);
}

void tick_isr()
{
    os_tick_handler();
    sbi_set_timer(get_ticks() + tick_cycles);
}