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
#include <dreamos.h>

os_size_t os_strlen(const char *str)
{
    os_size_t r = 0;

    while(*str++)
    {
        r++;
    }

    return r;
}

void os_memset(void *ptr,os_uint8_t value,os_size_t size)
{
    os_uint8_t *t_ptr = (os_uint8_t *)ptr;

    while(size--)
    {
        *(t_ptr++) = value;
    }
}

void os_memcpy(void *dst,const void *src,os_size_t size)
{
    os_uint8_t *t_dst = (os_uint8_t *)dst;
    os_uint8_t *t_src = (os_uint8_t *)src;

    while(size--)
    {
        *(t_dst++) = *(t_src++);
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

os_ssize_t os_strcmp(const char *str1,const char *str2)
{
    os_ssize_t ret = 0;

    while(!(ret = ((os_ssize_t)(*str1 - *str2))) && *str1)
	{
		str1++;
		str2++;
	}

	if(ret < 0)
	{
		return -1;
	}
	else if(ret > 0)
	{
		return 1;
	}

	return 0;
}

os_ssize_t os_memcmp(const void *buf1,const void *buf2,os_size_t len)
{
    os_ssize_t ret = 0;
    os_uint8_t *t_buf1 = (os_uint8_t *)buf1;
    os_uint8_t *t_buf2 = (os_uint8_t *)buf2;

    while((len--) && !(ret = ((os_ssize_t)(*t_buf1 - *t_buf2))))
	{
		t_buf1++;
        t_buf2++;
	}

	if(ret < 0)
	{
		return -1;
	}
	else if(ret > 0)
	{
		return 1;
	}

	return 0;
}