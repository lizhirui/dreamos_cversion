/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __STACKFRAME_H__
#define __STACKFRAME_H__

    #define STORE                   sd
    #define LOAD                    ld
    #define REGBYTES                8
    #define SAVE_ALL_REG_NUM        33

    .macro SAVE_ALL
        addi sp, sp, -SAVE_ALL_REG_NUM * REGBYTES

        STORE x1,   1 * REGBYTES(sp)

        csrr  x1, sstatus
        STORE x1,   2 * REGBYTES(sp)

        csrr  x1, sepc
        STORE x1, 0 * REGBYTES(sp)

        STORE x3,   3 * REGBYTES(sp)
        STORE x4,   4 * REGBYTES(sp)
        STORE x5,   5 * REGBYTES(sp)
        STORE x6,   6 * REGBYTES(sp)
        STORE x7,   7 * REGBYTES(sp)
        STORE x8,   8 * REGBYTES(sp)
        STORE x9,   9 * REGBYTES(sp)
        STORE x10, 10 * REGBYTES(sp)
        STORE x11, 11 * REGBYTES(sp)
        STORE x12, 12 * REGBYTES(sp)
        STORE x13, 13 * REGBYTES(sp)
        STORE x14, 14 * REGBYTES(sp)
        STORE x15, 15 * REGBYTES(sp)
        STORE x16, 16 * REGBYTES(sp)
        STORE x17, 17 * REGBYTES(sp)
        STORE x18, 18 * REGBYTES(sp)
        STORE x19, 19 * REGBYTES(sp)
        STORE x20, 20 * REGBYTES(sp)
        STORE x21, 21 * REGBYTES(sp)
        STORE x22, 22 * REGBYTES(sp)
        STORE x23, 23 * REGBYTES(sp)
        STORE x24, 24 * REGBYTES(sp)
        STORE x25, 25 * REGBYTES(sp)
        STORE x26, 26 * REGBYTES(sp)
        STORE x27, 27 * REGBYTES(sp)
        STORE x28, 28 * REGBYTES(sp)
        STORE x29, 29 * REGBYTES(sp)
        STORE x30, 30 * REGBYTES(sp)
        STORE x31, 31 * REGBYTES(sp)
        csrr t0, sscratch
        STORE t0, 32 * REGBYTES(sp)
    .endm

    .macro SAVE_ALL_AND_RESTORE_SSCRATCH
        SAVE_ALL
        addi t0, sp, SAVE_ALL_REG_NUM * REGBYTES
        csrw sscratch, t0
    .endm

    .macro RESTORE_ALL
            /* resw ra to sepc */
        LOAD x1,   0 * REGBYTES(sp)
        csrw sepc, x1

        LOAD x1,   2 * REGBYTES(sp)
        csrw sstatus, x1

        LOAD x1,   1 * REGBYTES(sp)

        LOAD x3,   3 * REGBYTES(sp)
        LOAD x4,   4 * REGBYTES(sp)
        LOAD x5,   5 * REGBYTES(sp)
        LOAD x6,   6 * REGBYTES(sp)
        LOAD x7,   7 * REGBYTES(sp)
        LOAD x8,   8 * REGBYTES(sp)
        LOAD x9,   9 * REGBYTES(sp)
        LOAD x10, 10 * REGBYTES(sp)
        LOAD x11, 11 * REGBYTES(sp)
        LOAD x12, 12 * REGBYTES(sp)
        LOAD x13, 13 * REGBYTES(sp)
        LOAD x14, 14 * REGBYTES(sp)
        LOAD x15, 15 * REGBYTES(sp)
        LOAD x16, 16 * REGBYTES(sp)
        LOAD x17, 17 * REGBYTES(sp)
        LOAD x18, 18 * REGBYTES(sp)
        LOAD x19, 19 * REGBYTES(sp)
        LOAD x20, 20 * REGBYTES(sp)
        LOAD x21, 21 * REGBYTES(sp)
        LOAD x22, 22 * REGBYTES(sp)
        LOAD x23, 23 * REGBYTES(sp)
        LOAD x24, 24 * REGBYTES(sp)
        LOAD x25, 25 * REGBYTES(sp)
        LOAD x26, 26 * REGBYTES(sp)
        LOAD x27, 27 * REGBYTES(sp)
        LOAD x28, 28 * REGBYTES(sp)
        LOAD x29, 29 * REGBYTES(sp)
        LOAD x30, 30 * REGBYTES(sp)
        LOAD x31, 31 * REGBYTES(sp)

        //restore user sp
        LOAD sp, 32 * REGBYTES(sp)
    .endm

    .macro RESTORE_SYS_GP
        .option push
        .option norelax
            la gp, __global_pointer$
        .option pop
    .endm

    .macro OPEN_INTERRUPT
        csrsi sstatus, 2
    .endm

    .macro CLOSE_INTERRUPT
        csrci sstatus, 2
    .endm

#endif