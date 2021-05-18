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
    size_t os_strlen(const char *str);
    void os_memset(uint8_t *ptr,uint8_t value,size_t size);
    void os_memcpy(uint8_t *dst,uint8_t *src,size_t size);
    void os_strcpy(char *dststr,const char *srcstr);

#endif