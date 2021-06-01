/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

    .section .text
    /*用于中断屏蔽*/
    .global bsp_interrupt_disable
bsp_interrupt_disable:
    csrrci a0, sstatus, 2
    andi a0, a0, 2
    srli a0, a0, 1
    ret

    /*用于中断开启*/
    .global bsp_interrupt_enable
bsp_interrupt_enable:
    slli a0, a0, 1
    csrrs x0, sstatus, a0
    ret
