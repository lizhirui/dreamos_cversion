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

#ifndef __OS_DEBUG_H__
#define __OS_DEBUG_H__

    #include <dreamos.h>

    void os_assert_handler(const char *ex_string,const char *func,os_size_t line);

    #define OS_ASSERT(condition)                                                         \
    do                                                                            \
    {                                                                             \
        if (!(condition))                                                                \
        {                                                                         \
            os_assert_handler(#condition,__FUNCTION__,__LINE__);                       \
        }                                                                         \
    }while(0)

    typedef struct os_symtab_header
    {
        os_size_t function_table_offset;
        os_size_t function_table_num;
        os_size_t object_table_offset;
        os_size_t object_table_num;
        os_size_t general_symbol_table_offset;
        os_size_t general_symbol_table_num;
        os_size_t string_table_offset;
        os_size_t string_table_size;
    }os_symtab_header;

    typedef struct os_symtab_item
    {
        os_size_t name_offset;
        os_size_t address;
        os_size_t size;
    }os_symtab_item;

    os_symtab_item *find_symbol_table(os_size_t symbol_table_addr,os_size_t symbol_num,os_size_t address);
    const char *get_symbol_name(os_symtab_item *symbol);
    void print_symbol(os_symtab_item *symbol,size_t address);
    void print_symbol_info(size_t address,os_bool_t function);
    void print_stacktrace(size_t epc,size_t fp);


#endif