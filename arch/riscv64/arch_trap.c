/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-05-20     lizhirui     add os debug support
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
    os_printf("\tra(x1) = 0x%p(",regs -> ra);print_symbol_info(regs -> ra,FALSE);os_printf(")\n");
    os_printf("\tuser_sp(x2) = 0x%p(",regs -> user_sp);print_symbol_info(regs -> user_sp,FALSE);os_printf(")\n");
    os_printf("\tgp(x3) = 0x%p(",regs -> gp);print_symbol_info(regs -> gp,FALSE);os_printf(")\n");
    os_printf("\ttp(x4) = 0x%p(",regs -> tp);print_symbol_info(regs -> tp,FALSE);os_printf(")\n");
    os_printf("Temporary Registers:\n");
    os_printf("\tt0(x5) = 0x%p(",regs -> t0);print_symbol_info(regs -> t0,FALSE);os_printf(")\n");
    os_printf("\tt1(x6) = 0x%p(",regs -> t1);print_symbol_info(regs -> t1,FALSE);os_printf(")\n");
    os_printf("\tt2(x7) = 0x%p(",regs -> t2);print_symbol_info(regs -> t2,FALSE);os_printf(")\n");
    os_printf("\tt3(x28) = 0x%p(",regs -> t3);print_symbol_info(regs -> t3,FALSE);os_printf(")\n");
    os_printf("\tt4(x29) = 0x%p(",regs -> t4);print_symbol_info(regs -> t4,FALSE);os_printf(")\n");
    os_printf("\tt5(x30) = 0x%p(",regs -> t5);print_symbol_info(regs -> t5,FALSE);os_printf(")\n");
    os_printf("\tt6(x31) = 0x%p(",regs -> t6);print_symbol_info(regs -> t6,FALSE);os_printf(")\n");
    os_printf("Saved Registers:\n");
    os_printf("\ts0/fp(x8) = 0x%p(",regs -> s0_fp);print_symbol_info(regs -> s0_fp,FALSE);os_printf(")\n");
    os_printf("\ts1(x9) = 0x%p(",regs -> s1);print_symbol_info(regs -> s1,FALSE);os_printf(")\n");
    os_printf("\ts2(x18) = 0x%p(",regs -> s2);print_symbol_info(regs -> s2,FALSE);os_printf(")\n");
    os_printf("\ts3(x19) = 0x%p(",regs -> s3);print_symbol_info(regs -> s3,FALSE);os_printf(")\n");
    os_printf("\ts4(x20) = 0x%p(",regs -> s4);print_symbol_info(regs -> s4,FALSE);os_printf(")\n");
    os_printf("\ts5(x21) = 0x%p(",regs -> s5);print_symbol_info(regs -> s5,FALSE);os_printf(")\n");
    os_printf("\ts6(x22) = 0x%p(",regs -> s6);print_symbol_info(regs -> s6,FALSE);os_printf(")\n");
    os_printf("\ts7(x23) = 0x%p(",regs -> s7);print_symbol_info(regs -> s7,FALSE);os_printf(")\n");
    os_printf("\ts8(x24) = 0x%p(",regs -> s8);print_symbol_info(regs -> s8,FALSE);os_printf(")\n");
    os_printf("\ts9(x25) = 0x%p(",regs -> s9);print_symbol_info(regs -> s9,FALSE);os_printf(")\n");
    os_printf("\ts10(x26) = 0x%p(",regs -> s10);print_symbol_info(regs -> s10,FALSE);os_printf(")\n");
    os_printf("\ts11(x27) = 0x%p(",regs -> s11);print_symbol_info(regs -> s11,FALSE);os_printf(")\n");
    os_printf("Function Arguments Registers:\n");
    os_printf("\ta0(x10) = 0x%p(",regs -> a0);print_symbol_info(regs -> a0,FALSE);os_printf(")\n");
    os_printf("\ta1(x11) = 0x%p(",regs -> a1);print_symbol_info(regs -> a1,FALSE);os_printf(")\n");
    os_printf("\ta2(x12) = 0x%p(",regs -> a2);print_symbol_info(regs -> a2,FALSE);os_printf(")\n");
    os_printf("\ta3(x13) = 0x%p(",regs -> a3);print_symbol_info(regs -> a3,FALSE);os_printf(")\n");
    os_printf("\ta4(x14) = 0x%p(",regs -> a4);print_symbol_info(regs -> a4,FALSE);os_printf(")\n");
    os_printf("\ta5(x15) = 0x%p(",regs -> a5);print_symbol_info(regs -> a5,FALSE);os_printf(")\n");
    os_printf("\ta6(x16) = 0x%p(",regs -> a6);print_symbol_info(regs -> a6,FALSE);os_printf(")\n");
    os_printf("\ta7(x17) = 0x%p(",regs -> a7);print_symbol_info(regs -> a7,FALSE);os_printf(")\n");
    os_printf("sstatus = 0x%p\n",regs -> sstatus);
    os_printf("---------------------------------Dump OK--------------------------------\n");
    print_stacktrace(sepc,regs -> s0_fp);
    while(1);
}