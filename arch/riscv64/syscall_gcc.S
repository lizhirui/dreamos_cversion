/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-07-06     lizhirui     implement syscall_entry
 */

#include "encoding.h"
#include "stackframe.h"
    .option norelax
    .section .text
    .align 2
    .global syscall_entry
    
syscall_entry:
    //restore a little context only
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    LOAD gp, 2 * REGBYTES(sp)
    addi sp, sp, 3 * REGBYTES

    //backup context
    SAVE_ALL_AND_RESTORE_SSCRATCH

    //restore gp
    RESTORE_SYS_GP

    //swap to thread kernel stack
    csrr t0, sstatus
    andi t0, t0, 0x100
    beqz t0, __restore_sp_from_tcb

__restore_sp_from_sscratch:
    csrr t0, sscratch
    j __move_stack_context

__restore_sp_from_tcb:
    jal os_task_get_current_task
    jal os_task_get_kernel_stack_top
    mv t0, a0

__move_stack_context:
    mv t1, sp//src
    mv sp, t0//switch stack
    addi sp, sp, -33 * REGBYTES
    //copy context
    li s0, 33//cnt
    mv t2, sp//dst

copy_context_loop:
    LOAD t0, 0(t1)
    STORE t0, 0(t2)
    addi s0, s0, -1
    addi t1, t1, 8
    addi t2, t2, 8
    bnez s0, copy_context_loop

    mv a0, sp
    OPEN_INTERRUPT
    call arch_syscall_handler
    CLOSE_INTERRUPT

.global syscall_exit
syscall_exit:
    //restore context
    RESTORE_ALL
    sret
