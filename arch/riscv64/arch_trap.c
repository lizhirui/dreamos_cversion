/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#include <dreamos.h>
#include <encoding.h>

const char *get_exception_name(enum exception_type exception_type)
{
    switch(exception_type)
    {
        case EXCEPTION_INSTRUCTION_ADDRESS_MISALIGNED:return "Instruction Address Misaligned";
        case EXCEPTION_INSTRUCTION_ACCESS_FAULT:return "Instruction Access Fault";
        case EXCEPTION_ILLEGAL_INSTRUCTION:return "Illegal Instruction";
        case EXCEPTION_BREAKPOINT:return "Breakpoint";
        case EXCEPTION_LOAD_ADDRESS_MISALIGNED:return "Load Address Misaligned";
        case EXCEPTION_LOAD_ACCESS_FAULT:return "Load Access Fault";
        case EXCEPTION_STORE_AMO_ADDRESS_MISALIGNED:return "Store/AMO Address Misaligned";
        case EXCEPTION_STORE_AMO_ACCESS_FAULT:return "Store/AMO Access Fault";
        case EXCEPTION_ENVIRONMENT_CALL_FROM_UMODE:return "Environment call from U-mode";
        case EXCEPTION_ENVIRONMENT_CALL_FROM_SMODE:return "Environment call from S-mode";
        case EXCEPTION_RESERVED_10:return "Reserved-10";
        case EXCEPTION_RESERVED_11:return "Reserved-11";
        case EXCEPTION_INSTRUCTION_PAGE_FAULT:return "Instruction Page Fault";
        case EXCEPTION_LOAD_PAGE_FAULT:return "Load Page Fault";
        case EXCEPTION_RESERVED_14:return "Reserved-14";
        case EXCEPTION_STORE_AMO_PAGE_FAULT:return "Store/AMO Page Fault";
        default:return "Unknown Exception";
    }
}

const char *get_interrupt_name(enum interrupt_type interrupt_type)
{
    switch(interrupt_type)
    {
        case INTERRUPT_USER_SOFTWARE:return "User Software Interrupt";
        case INTERRUPT_SUPERVISOR_SOFTWARE:return "Supervisor Software Interrupt";
        case INTERRUPT_RESERVED_2:return "Reserved-2";
        case INTERRUPT_RESERVED_3:return "Reserved-3";
        case INTERRUPT_USER_TIMER:return "User Timer Interrupt";
        case INTERRUPT_SUPERVISOR_TIMER:return "Supervisor Timer Interrupt";
        case INTERRUPT_RESERVED_6:return "Reserved-6";
        case INTERRUPT_RESERVED_7:return "Reserved-7";
        case INTERRUPT_USER_EXTERNAL:return "User External Interrupt";
        case INTERRUPT_SUPERVISOR_EXTERNAL:return "Supervisor External Interrupt";
        case INTERRUPT_RESERVED_10:return "Reserved-10";
        case INTERRUPT_RESERVED_11:return "Reserved-11";
        default:return "Unknown Interrupt";
    }
}

const char *get_trap_name(size_t scause)
{
    if(SCAUSE_IS_INTERRUPT(scause))
    {
        return get_interrupt_name((enum interrupt_type)SCAUSE_GET_ID(scause));
    }
    else
    {
        return get_exception_name((enum exception_type)SCAUSE_GET_ID(scause));
    }
}

bool_t bsp_interrupt_handler(enum interrupt_type interrupt_type);

void trap_handler(size_t scause,size_t stval,size_t sepc,struct TrapFrame *regs)
{
    if(SCAUSE_IS_INTERRUPT(scause))
    {
        if(bsp_interrupt_handler((enum interrupt_type)SCAUSE_GET_ID(scause)))
        {
            return;
        }
    }

    os_printf("Unhandled %s %ld:%s\n",SCAUSE_IS_INTERRUPT(scause) ? "Interrupt" : "Exception",SCAUSE_GET_ID(scause),get_trap_name(scause));
    os_printf("scause = 0x%p\tstval = 0x%p\tsepc = 0x%p\n\n",scause,stval,sepc);
    os_printf("-----------------------------Dump Registers-----------------------------\n");
    os_printf("Function Registers:\n");
    os_printf("\tra(x1) = 0x%p\tuser_sp = 0x%p\n",regs -> ra,regs -> user_sp_exc_stack);
    os_printf("\tgp(x3) = 0x%p\ttp(x4) = 0x%p\n",regs -> gp,regs -> tp);
    os_printf("Temporary Registers:\n");
    os_printf("\tt0(x5) = 0x%p\tt1(x6) = 0x%p\n",regs -> t0,regs -> t1);
    os_printf("\tt2(x7) = 0x%p\n",regs -> t2);
    os_printf("\tt3(x28) = 0x%p\tt4(x29) = 0x%p\n",regs -> t3,regs -> t4);
    os_printf("\tt5(x30) = 0x%p\tt6(x31) = 0x%p\n",regs -> t5,regs -> t6);
    os_printf("Saved Registers:\n");
    os_printf("\ts0/fp(x8) = 0x%p\ts1(x9) = 0x%p\n",regs -> s0_fp,regs -> s1);
    os_printf("\ts2(x18) = 0x%p\ts3(x19) = 0x%p\n",regs -> s2,regs -> s3);
    os_printf("\ts4(x20) = 0x%p\ts5(x21) = 0x%p\n",regs -> s4,regs -> s5);
    os_printf("\ts6(x22) = 0x%p\ts7(x23) = 0x%p\n",regs -> s6,regs -> s7);
    os_printf("\ts8(x24) = 0x%p\ts9(x25) = 0x%p\n",regs -> s8,regs -> s9);
    os_printf("\ts10(x26) = 0x%p\ts11(x27) = 0x%p\n",regs -> s10,regs -> s11);
    os_printf("Function Arguments Registers:\n");
    os_printf("\ta0(x10) = 0x%p\ta1(x11) = 0x%p\n",regs -> a0,regs -> a1);
    os_printf("\ta2(x12) = 0x%p\ta3(x13) = 0x%p\n",regs -> a2,regs -> a3);
    os_printf("\ta4(x14) = 0x%p\ta5(x15) = 0x%p\n",regs -> a4,regs -> a5);
    os_printf("\ta6(x16) = 0x%p\ta7(x17) = 0x%p\n",regs -> a6,regs -> a7);
    os_printf("sstatus = 0x%p\n",regs -> sstatus);
    os_printf("---------------------------------Dump OK--------------------------------\n");
    while(1);
}