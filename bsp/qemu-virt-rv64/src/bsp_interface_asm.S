/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#define __ASSEMBLY__
#include <arch_syscall.h>

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

    .global user_entry_code
user_entry_code:
    //li a7, __NR_write
    //ecall
    //j user_entry_code
    li a7, __NR_execve
    lla a0, __path
    ecall

    .global user_entry_code_end
user_entry_code_end:

__path: .string "/test.elf"