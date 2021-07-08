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

#include "encoding.h"
#include "stackframe.h"
#include "osconfig.h"
#include "arch_mmu.h"
    
    .section .text.entry
    .extern __stack_default
    .extern __stack_interrupt_cpu0
    .extern trap_entry
    .global _start
_start:
    csrw sie, 0
    csrw sip, 0

    lla t0, trap_entry
    csrw stvec, t0

    /*disable FPU*/
    li t0, SSTATUS_FS
    csrc sstatus, t0
    RESTORE_SYS_GP
    lla sp, __stack_default
    lla t0, __stack_interrupt_cpu0
    csrw sscratch, t0

    li x1, 0
    li x4, 0
    li x5, 0
    li x6, 0
    li x7, 0
    li x8, 0
    li x9, 0
    li x10, 0
    li x11, 0
    li x12, 0
    li x13, 0
    li x14, 0
    li x15, 0
    li x16, 0
    li x17, 0
    li x18, 0
    li x19, 0
    li x20, 0
    li x21, 0
    li x22, 0
    li x23, 0
    li x24, 0
    li x25, 0
    li x26, 0
    li x27, 0
    li x28, 0
    li x29, 0
    li x30, 0
    li x31, 0

clear_bss:
    bge t0, t1, clear_bss_exit
    sd x0, 0(t0)
    addi t0, t0, 8
    j clear_bss

clear_bss_exit:
    jal os_mmu_preinit
    jal __enable_mmu
    jal os_mmu_preinit_secondary
    j main

.global __enable_mmu
__enable_mmu:
    addi sp, sp, -24
    sd s0, 0(sp)
    sd s1, 8(sp)
    sd s2, 16(sp)
    lla s2, enter_virtual_address_space
    li t1, OS_MMU_KERNEL_VA_PA_OFFSET
    add s2, s2, t1

    lla t0, __stack_interrupt_cpu0
    add t0, t0, t1
    csrw sscratch, t0
    
    mv s0, sp
    add s0, s0, t1

    mv s1, ra
    add s1, s1, t1

    lla a0, kernel_jump_pagetable
    jal os_mmu_switch
    jr s2

enter_virtual_address_space:
    RESTORE_SYS_GP
    mv sp, s0

    lla t0, trap_entry
    csrw stvec, t0

    mv ra, s1
    ld s0, 0(sp)
    ld s1, 8(sp)
    ld s2, 16(sp)
    addi sp, sp, 24
    ret

    .global enter_user_space
enter_user_space:
    csrci sstatus, 8//set sstatus.spp = 0
    csrw sepc, a0
    sret