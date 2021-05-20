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

typedef struct os_symtab_header
{
    size_t function_table_offset;
    size_t function_table_num;
    size_t object_table_offset;
    size_t object_table_num;
    size_t general_symbol_table_offset;
    size_t general_symbol_table_num;
    size_t string_table_offset;
    size_t string_table_size;
}os_symtab_header;

typedef struct os_symtab_item
{
    size_t name_offset;
    size_t address;
    size_t size;
}os_symtab_item;

extern size_t _osdebug_start;

static os_symtab_header *symtab_header = (os_symtab_header *)&_osdebug_start;

os_symtab_item *find_symbol_table(size_t symbol_table_addr,size_t symbol_num,size_t address)
{
    size_t left = 0;
    size_t right = symbol_num;
    os_symtab_item *sym_table = (os_symtab_item *)((size_t)&_osdebug_start + symbol_table_addr);

    while(left < right)
    {
        size_t mid = (left + right) >> 1;
        //os_printf("left = %d,right = %d,mid = %d\n",left,right,mid);

        if(address < sym_table[mid].address)
        {
            right = mid;

            while((right < symbol_num) && ((right - 1) >= 0) && (sym_table[right].address == sym_table[right - 1].address))
            {
                right--;
            }
        }
        else if(address == sym_table[mid].address)
        {
            left = mid + 1;
            break;
        }
        else
        {
            left = mid;

            while((left >= 0) && ((left + 1) < symbol_num) && (sym_table[left].address == sym_table[left + 1].address))
            {
                left++;
            }

            left++;
        }
    }    

    left--;

    if(left == ((size_t)-1))
    {
        return NULL;
    }

    while((left < symbol_num) && ((left - 1) >= 0) && (sym_table[left].address == sym_table[left - 1].address))
    {
        left--;
    }

    return &sym_table[left];
}

const char *get_symbol_name(os_symtab_item *symbol)
{
    return (const char *)((size_t)&_osdebug_start + symtab_header -> string_table_offset + symbol -> name_offset);
}

void print_symbol(os_symtab_item *symbol,size_t address)
{
    os_printf("<%s(0x%p)",get_symbol_name(symbol),symbol -> address);

    if(symbol -> size)
    {
        os_printf(" : 0x%x",symbol -> size);
    }

    if(address > symbol -> address)
    {
        os_printf(" + 0x%x",address - symbol -> address);
    }

    os_printf(">");
}

void print_symbol_info(size_t address,bool_t function)
{
    os_symtab_item *function_symbol = find_symbol_table(symtab_header -> function_table_offset,symtab_header -> function_table_num,address);
    os_symtab_item *object_symbol = find_symbol_table(symtab_header -> object_table_offset,symtab_header -> object_table_num,address);
    os_symtab_item *general_symbol = find_symbol_table(symtab_header -> general_symbol_table_offset,symtab_header -> general_symbol_table_num,address);
    const char *dot = "";
    bool_t valid = FALSE;
    
    if(function)
    {
        while(function_symbol != NULL)
        {
            if((function_symbol -> address + function_symbol -> size) > address)
            {
                os_printf(dot);
                print_symbol(function_symbol,address);
                dot = ",";
                valid = TRUE;
            }
            
            if(((size_t)(function_symbol + 1)) >= (((size_t)&_osdebug_start) + symtab_header -> function_table_offset + symtab_header -> function_table_num * sizeof(os_symtab_item)))
            {
                break;
            }

            if(function_symbol[0].address == function_symbol[1].address)
            {
                function_symbol++;
            }
            
            break;
        }

        if(!valid)
        {
            while(general_symbol != NULL)
            {
                os_printf(dot);
                print_symbol(general_symbol,address);
                dot = ",";
                valid = TRUE;
                
                if(((size_t)(general_symbol + 1)) >= (((size_t)&_osdebug_start) + symtab_header -> general_symbol_table_offset + symtab_header -> general_symbol_table_num * sizeof(os_symtab_item)))
                {
                    break;
                }

                if(general_symbol[0].address == general_symbol[1].address)
                {
                    general_symbol++;
                }

                break;
            }

            while(object_symbol != NULL)
            {
                if((object_symbol -> address + object_symbol -> size) > address)
                {
                    os_printf(dot);
                    print_symbol(object_symbol,address);
                    dot = ",";
                    valid = TRUE;
                }
                
                if(((size_t)(object_symbol + 1)) >= (((size_t)&_osdebug_start) + symtab_header -> object_table_offset + symtab_header -> object_table_num * sizeof(os_symtab_item)))
                {
                    break;
                }

                if(object_symbol[0].address == object_symbol[1].address)
                {
                    object_symbol++;
                }

                break;
            }
        }
    }
    else
    {
        while(object_symbol != NULL)
        {
            if((object_symbol -> address + object_symbol -> size) > address)
            {
                os_printf(dot);
                print_symbol(object_symbol,address);
                dot = ",";
                valid = TRUE;
            }
            
            if(((size_t)(object_symbol + 1)) >= (((size_t)&_osdebug_start) + symtab_header -> object_table_offset + symtab_header -> object_table_num * sizeof(os_symtab_item)))
            {
                break;
            }

            if(object_symbol[0].address == object_symbol[1].address)
            {
                object_symbol++;
            }

            break;
        }

        if(!valid)
        {
            while(general_symbol != NULL)
            {
                os_printf(dot);
                print_symbol(general_symbol,address);
                dot = ",";
                valid = TRUE;
                
                if(((size_t)(general_symbol + 1)) >= (((size_t)&_osdebug_start) + symtab_header -> general_symbol_table_offset + symtab_header -> general_symbol_table_num * sizeof(os_symtab_item)))
                {
                    break;
                }

                if(general_symbol[0].address == general_symbol[1].address)
                {
                    general_symbol++;
                }

                break;
            }

            while(function_symbol != NULL)
            {
                if((function_symbol -> address + function_symbol -> size) > address)
                {
                    os_printf(dot);
                    print_symbol(function_symbol,address);
                    dot = ",";
                    valid = TRUE;
                }
                
                if(((size_t)(function_symbol + 1)) >= (((size_t)&_osdebug_start) + symtab_header -> function_table_offset + symtab_header -> function_table_num * sizeof(os_symtab_item)))
                {
                    break;
                }

                if(function_symbol[0].address == function_symbol[1].address)
                {
                    function_symbol++;
                }
                
                break;
            }
        }
    }

    if(dot == "")
    {
        os_printf("<Unknown Symbol>");
    }
}

void print_stacktrace(size_t epc,size_t fp)
{
    os_printf("-----------------------------Dump Stacktrace----------------------------\n\n");
    size_t sp = fp;
    size_t i = 0;

    os_printf("address 0x%p(",epc);
    print_symbol_info(epc,TRUE);
    os_printf(")\n\n");

    while(1)
    {
        if((sp >= MEMORY_BASE) && (sp < (MEMORY_BASE + MEMORY_SIZE)))
        {
            //os_printf("%d: 0x%p\n",i,sp);
            size_t *stack = (size_t *)(sp - sizeof(size_t) * 2);
            size_t ra = stack[1];

            if(!ra)
            {
                break;
            }

            os_printf("return to 0x%p(",ra);
            print_symbol_info(ra,TRUE);
            os_printf(")\n\n");
            //os_printf("ra = 0x%p,fp = 0x%p\n",stack[1],stack[0]);
            sp = stack[0];
            i++;
        }
        else
        {
            break;
        }
    }

    os_printf("---------------------------------Dump OK--------------------------------\n");
}

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