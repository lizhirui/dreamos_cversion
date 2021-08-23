/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 * 2021-07-07     lizhirui     add os_list_node_init
 */

// @formatter:off
#ifndef __LIST_H__
#define __LIST_H__

    //列表节点
    typedef struct os_list_node
    {
        struct os_list_node *prev;//上一个节点
        struct os_list_node *next;//下一个节点
    }os_list_node_t,*os_list_node_p;

    //获取某个指针对应的原始结构体指针
    #define os_container_of(ptr,type,member) ((type *)((char *)(ptr) - (size_t)(&((type *)0) -> member)))

    //列表初始化
    #define os_list_init(list) do{(list).prev = &(list);(list).next = (&list);}while(0)
    //列表节点初始化
    #define os_list_node_init(list_node_ptr) do{(list_node_ptr) -> prev = OS_NULL;(list_node_ptr) -> next = OS_NULL;}while(0)
    //获取一个列表节点对应的原始数据结构体指针
    #define os_list_entry(list_node_ptr,type,member) os_container_of(list_node_ptr,type,member)
    //判断一个节点是否为空节点
    #define os_list_node_empty(list_node_ptr) (((list_node_ptr) -> prev == OS_NULL) && ((list_node_ptr) -> next == OS_NULL))

    //遍历列表（不支持删除操作）
    #define os_list_entry_foreach(list,type,member,entry_variable,body) \
    { \
        os_list_node_p cur_node = (list).next; \
        \
        for(;cur_node != &(list);cur_node = cur_node -> next) \
        { \
            type *entry_variable = os_list_entry(cur_node,type,member); \
            {body} \
        } \
    } \

    //遍历列表（支持删除操作）
    #define os_list_entry_foreach_safe(list,type,member,entry_variable,body) \
    { \
        os_list_node_p cur_node = (list).next; \
        \
        for(;cur_node != &(list);) \
        { \
            type *entry_variable = os_list_entry(cur_node,type,member); \
            cur_node = cur_node -> next; \
            {body} \
        } \
    } \

    //在指定节点之前插入一个节点
    #define os_list_node_insert_before(list_node_ptr,reference_node_ptr) \
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

    //在指定节点之后插入一个节点
    #define os_list_node_insert_after(list_node_ptr,reference_node_ptr) \
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
        
    //从列表中移除一个节点
    #define os_list_node_remove(list_node_ptr) \
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

    //将一个节点插入到列表的头部
    #define os_list_insert_head(list,os_list_node) os_list_node_insert_after(os_list_node,&(list))
    //将一个节点插入到列表的尾部
    #define os_list_insert_tail(list,os_list_node) os_list_node_insert_before(os_list_node,&(list))
    //获取列表的头节点
    #define os_list_get_head(list) ((list).next)
    //获取列表的尾结点
    #define os_list_get_tail(list) ((list).prev)
    //判断列表是否为空
    #define os_list_empty(list) (((list).next == &list) && ((list).prev == &list))

#endif