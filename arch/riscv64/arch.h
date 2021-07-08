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
#ifndef __ARCH_H__
#define __ARCH_H__

    #include "encoding.h"

    #define SCAUSE_IS_INTERRUPT(scause) ((scause) >> 63)
    #define SCAUSE_GET_ID(scause) (UMASK_VALUE(scause,1UL << 63))

    #define SYNC() do{asm volatile("fence;fence.i");}while(0)
    #define SYNC_DATA() do{asm volatile("fence");}while(0)
    #define SYNC_INSTRUCTION() do{asm volatile("fence.i");}while(0)

    #include "arch_trap.h"
    #include "arch_mmu.h"
    #include "arch_syscall.h"

#endif