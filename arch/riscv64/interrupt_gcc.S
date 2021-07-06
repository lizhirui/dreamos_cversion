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
    .section .text
    .extern syscall_entry
    .global trap_entry
    .align 4
trap_entry:
    //backup user sp,switch to interrupt stack for current cpu
    csrrw sp, sscratch, sp
    
    //backup a little context only
    addi sp, sp, -3 * REGBYTES
    STORE t0, 0 * REGBYTES(sp)
    STORE t1, 1 * REGBYTES(sp)
    STORE gp, 2 * REGBYTES(sp)

    //restore gp
    RESTORE_SYS_GP
    
    //check syscall
    csrr t0, scause
    li t1, 8//environment call from u-mode
    beq t0, t1, syscall_entry

    //restore a little context only
    LOAD t0, 0 * REGBYTES(sp)
    LOAD t1, 1 * REGBYTES(sp)
    LOAD gp, 2 * REGBYTES(sp)
    addi sp, sp, 3 * REGBYTES

    //backup context
    SAVE_ALL_AND_RESTORE_SSCRATCH

    //restore gp
    RESTORE_SYS_GP

    //prepare arguments for trap handler
    csrr a0, scause
    csrrc a1, stval, zero
    csrr a2, sepc
    mv a3, sp

    call trap_handler

    la s0, need_lazy_task_switch
    LOAD s1, 0(s0)
    beqz s1, _interrupt_exit

_switch_interrupt:
    //swap to thread kernel stack
    csrr t0, sstatus
    andi t0, t0, 0x100
    beqz t0, __restore_sp_from_tcb_interrupt

__restore_sp_from_context_interrupt:
    LOAD t0, 32 * REGBYTES(sp)
    j __move_stack_context_interrupt

__restore_sp_from_tcb_interrupt:
    la a0, lazy_old_task
    LOAD a0, 0(a0)
    jal os_task_get_kernel_stack_top
    mv t0, a0

__move_stack_context_interrupt:
    mv t1, sp//src
    mv sp, t0//switch stack
    addi sp, sp, -33 * REGBYTES
    //copy context
    li s0, 33//cnt
    mv t2, sp//dst

copy_context_loop_interrupt:
    LOAD t0, 0(t1)
    STORE t0, 0(t2)
    addi s0, s0, -1
    addi t1, t1, 8
    addi t2, t2, 8
    bnez s0, copy_context_loop_interrupt

    la a0, lazy_old_task
    LOAD a0, 0(a0)
    la a1, lazy_next_task
    LOAD a1, 0(a1)
    la t0, need_lazy_task_switch
    STORE x0, 0(t0)
    STORE sp, 0(a0)
    LOAD  sp, 0(a1)

    mv a0, a1
    jal os_task_switch_vtable

_interrupt_exit:
    //restore context
    RESTORE_ALL
    sret
