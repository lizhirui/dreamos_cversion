/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#include "encoding.h"
#include "stackframe.h"
    
/*用于arch实现任务切换*/
    .section .text
    .global arch_task_switch
arch_task_switch:
    beqz a0, _switch_to
    
    mv t2, sp
    li t0, 0x120
    csrs sstatus, t0
    csrw sepc, ra
    SAVE_ALL

    STORE t2, 32 * REGBYTES(sp)
    STORE sp, (a0)

_switch_to:
    LOAD sp, (a1)
    RESTORE_ALL
    sret