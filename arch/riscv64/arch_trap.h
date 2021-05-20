/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __ARCH_TRAP_H__
#define __ARCH_TRAP_H__

    #include <dreamos.h>

    struct TrapFrame
    {
        size_t sepc;
        size_t ra;
        size_t sstatus;
        size_t gp;
        size_t tp;
        size_t t0;
        size_t t1;
        size_t t2;
        size_t s0_fp;
        size_t s1;
        size_t a0;
        size_t a1;
        size_t a2;
        size_t a3;
        size_t a4;
        size_t a5;
        size_t a6;
        size_t a7;
        size_t s2;
        size_t s3;
        size_t s4;
        size_t s5;
        size_t s6;
        size_t s7;
        size_t s8;
        size_t s9;
        size_t s10;
        size_t s11;
        size_t t3;
        size_t t4;
        size_t t5;
        size_t t6;
        size_t user_sp;
    };

    enum exception_type
    {
        EXCEPTION_INSTRUCTION_ADDRESS_MISALIGNED = 0,
        EXCEPTION_INSTRUCTION_ACCESS_FAULT,
        EXCEPTION_ILLEGAL_INSTRUCTION,
        EXCEPTION_BREAKPOINT,
        EXCEPTION_LOAD_ADDRESS_MISALIGNED,
        EXCEPTION_LOAD_ACCESS_FAULT,
        EXCEPTION_STORE_AMO_ADDRESS_MISALIGNED,
        EXCEPTION_STORE_AMO_ACCESS_FAULT,
        EXCEPTION_ENVIRONMENT_CALL_FROM_UMODE,
        EXCEPTION_ENVIRONMENT_CALL_FROM_SMODE,
        EXCEPTION_RESERVED_10,
        EXCEPTION_RESERVED_11,
        EXCEPTION_INSTRUCTION_PAGE_FAULT,
        EXCEPTION_LOAD_PAGE_FAULT,
        EXCEPTION_RESERVED_14,
        EXCEPTION_STORE_AMO_PAGE_FAULT
    };

    enum interrupt_type
    {
        INTERRUPT_USER_SOFTWARE = 0,
        INTERRUPT_SUPERVISOR_SOFTWARE,
        INTERRUPT_RESERVED_2,
        INTERRUPT_RESERVED_3,
        INTERRUPT_USER_TIMER,
        INTERRUPT_SUPERVISOR_TIMER,
        INTERRUPT_RESERVED_6,
        INTERRUPT_RESERVED_7,
        INTERRUPT_USER_EXTERNAL,
        INTERRUPT_SUPERVISOR_EXTERNAL,
        INTERRUPT_RESERVED_10,
        INTERRUPT_RESERVED_11
    };

    const char *get_exception_name(enum exception_type exception_type);
    const char *get_interrupt_name(enum interrupt_type interrupt_type);
    const char *get_trap_name(size_t scause);

#endif