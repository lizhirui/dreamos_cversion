/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __PHYPAGE_H__
#define __PHYPAGE_H__

    void *phypage_alloc(size_t size);
    void phypage_free(void *addr,size_t old_size);
    size_t get_allocated_page_count();
    size_t get_total_page_count();
    size_t get_free_page_count();
    void phypage_init();

#endif