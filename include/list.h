/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __LIST_H__
#define __LIST_H__

    typedef struct list_node
    {
        struct list_node *prev;
        struct list_node *next;
    }list_node;

    #define container_of(ptr,type,member) ((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

    #define list_init(list) do{(list).prev = &(list);(list).next = (&list);}while(0)
    #define list_entry(list_node_ptr,type,member) container_of(list_node_ptr,type,member)

    #define list_entry_foreach(list,type,member,entry_variable,body) \
    { \
        type *cur_node = (list).next; \
        \
        for(;cur_node != &(list);cur_node = cur_node -> next) \
        { \
            type *entry_variable = list_entry(cur_node,type,member); \
            {body} \
        } \
    } \

    #define list_entry_foreach_safe(list,type,member,entry_variable,body) \
    { \
        type *cur_node = (list).next; \
        \
        for(;cur_node != &(list);) \
        { \
            type *entry_variable = list_entry(cur_node,type,member); \
            cur_node = cur_node -> next \
            {body} \
        } \
    } \

    #define list_node_insert_before(list_node_ptr,reference_node_ptr) \
        do \
        { \
            (list_node_ptr) -> next = (reference_node_ptr); \
            (list_node_ptr) -> prev = (reference_node_ptr) -> prev; \
            (reference_node_ptr) -> prev = (list_node_ptr); \
             \
            if((list_node_ptr) -> prev != OS_NULL) \
            { \
                (list_node_ptr) -> prev -> next = (list_node_ptr); \
            } \
        }while(0);

    #define list_node_insert_after(list_node_ptr,reference_node_ptr) \
        do \
        { \
            (list_node_ptr) -> next = (reference_node_ptr) -> next; \
            (list_node_ptr) -> prev = (reference_node_ptr); \
            (reference_node_ptr) -> next = (list_node_ptr); \
             \
            if((list_node_ptr) -> next != OS_NULL) \
            { \
                (list_node_ptr) -> next -> prev = (list_node_ptr); \
            } \
        }while(0);
        

    #define list_node_remove(list_node_ptr) \
        do \
        { \
            if((list_node_ptr) -> prev != OS_NULL) \
            { \
                (list_node_ptr) -> prev -> next = (list_node_ptr) -> next; \
            } \
             \
            if((list_node_ptr) -> next != OS_NULL) \
            { \
                (list_node_ptr) -> next -> prev = (list_node_ptr) -> prev; \
            }; \
             \
            (list_node_ptr) -> prev = OS_NULL; \
            (list_node_ptr) -> next = OS_NULL; \
        }while(0)

    #define list_insert_head(list,list_node) list_node_insert_after(list_node,&list)
    #define list_insert_tail(list,list_node) list_node_insert_before(list_node,&list)
    #define list_get_head(list) (list.next)
    #define list_get_tail(list) (list.prev)
    #define list_empty(list) ((list.next == &list) && (list.prev == &list))

#endif