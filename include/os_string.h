/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#ifndef __OS_STRING_H__
#define __OS_STRING_H__

    #include <dreamos.h>
    os_size_t os_strlen(const char *str);
    void os_memset(os_uint8_t *ptr,os_uint8_t value,os_size_t size);
    void os_memcpy(os_uint8_t *dst,os_uint8_t *src,os_size_t size);
    void os_strcpy(char *dststr,const char *srcstr);

#endif