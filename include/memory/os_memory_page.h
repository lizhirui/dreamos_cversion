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
#ifndef __OS_MEMORY_PAGE_H__
#define __OS_MEMORY_PAGE_H__

    void *os_memory_page_alloc(os_size_t size);
    void os_memory_page_free(void *addr);
    os_size_t os_memory_page_get_allocated_page_count();
    os_size_t os_memory_page_get_total_page_count();
    os_size_t os_memory_page_get_free_page_count();
    void os_memory_page_init();

#endif