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
#ifndef __ARCH_TRAP_H__
#define __ARCH_TRAP_H__

    #include <dreamos.h>

    struct TrapFrame
    {
        os_size_t sepc;
        os_size_t ra;
        os_size_t sstatus;
        os_size_t gp;
        os_size_t tp;
        os_size_t t0;
        os_size_t t1;
        os_size_t t2;
        os_size_t s0_fp;
        os_size_t s1;
        os_size_t a0;
        os_size_t a1;
        os_size_t a2;
        os_size_t a3;
        os_size_t a4;
        os_size_t a5;
        os_size_t a6;
        os_size_t a7;
        os_size_t s2;
        os_size_t s3;
        os_size_t s4;
        os_size_t s5;
        os_size_t s6;
        os_size_t s7;
        os_size_t s8;
        os_size_t s9;
        os_size_t s10;
        os_size_t s11;
        os_size_t t3;
        os_size_t t4;
        os_size_t t5;
        os_size_t t6;
        os_size_t user_sp;
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
    const char *get_trap_name(os_size_t scause);

#endif