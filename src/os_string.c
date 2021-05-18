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

size_t os_strlen(const char *str)
{
    size_t r = 0;

    while(*str++)
    {
        r++;
    }

    return r;
}

void os_memset(uint8_t *ptr,uint8_t value,size_t size)
{
    while(size--)
    {
        *(ptr++) = value;
    }
}

void os_memcpy(uint8_t *dst,uint8_t *src,size_t size)
{
    while(size--)
    {
        *(dst++) = *(src++);
    }
}

void os_strcpy(char *dststr,const char *srcstr)
{
    while(*srcstr)
    {
        *(dststr++) = *(srcstr++);
    }

    *dststr = '\0';
}