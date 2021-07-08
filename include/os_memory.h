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
#ifndef __OS_MEMORY_H__
#define __OS_MEMORY_H__

    #include <memory/os_memory_page.h>
    #include <memory/os_memory_slub.h>

    void os_memory_init();
    os_bool_t os_memory_is_initialized();
    void *os_memory_alloc(os_size_t size);
    void os_memory_free(void *mem);
    os_size_t os_get_allocated_memory();
    os_size_t os_get_total_memory();
    os_size_t os_get_free_memory();

#endif