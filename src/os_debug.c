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

void os_assert_handler(const char *ex_string,const char *func,size_t line)
{
    os_printf("(%s) assertion failed at function:%s, line number:%d \n",ex_string,func,line);
    while(1);
}

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
        os_printf(" : 0x%x>",symbol -> size);
    }
    else
    {
        os_printf(">");
    }

    if(address > symbol -> address)
    {
        os_printf(" + 0x%x",address - symbol -> address);
    }
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