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

/*!
 * 获取字符串长度
 * @param str 字符串指针
 * @return 字符串长度
 */
os_size_t os_strlen(const char *str)
{
    os_size_t r = 0;

    while(*str++)
    {
        r++;
    }

    return r;
}

/*!
 * 按字节写入一段内存
 * @param ptr 内存指针
 * @param value 要写入的值
 * @param size 内存大小
 */
void os_memset(void *ptr,os_uint8_t value,os_size_t size)
{
    os_uint8_t *t_ptr = (os_uint8_t *)ptr;

    while(size--)
    {
        *(t_ptr++) = value;
    }
}

/*!
 * 内存拷贝
 * @param dst 目标内存指针
 * @param src 源内存指针
 * @param size 内存大小
 */
void os_memcpy(void *dst,const void *src,os_size_t size)
{
    os_uint8_t *t_dst = (os_uint8_t *)dst;
    os_uint8_t *t_src = (os_uint8_t *)src;

    while(size--)
    {
        *(t_dst++) = *(t_src++);
    }
}

/*!
 * 字符串拷贝
 * @param dststr 目标字符串指针
 * @param srcstr 源字符串指针
 */
void os_strcpy(char *dststr,const char *srcstr)
{
    while(*srcstr)
    {
        *(dststr++) = *(srcstr++);
    }

    *dststr = '\0';
}

/*!
 * 字符串比较
 * @param str1 字符串1指针
 * @param str2 字符串2指针
 * @return 若相等，则返回0，若字符串1某个字符更小，则返回-1，否则返回1
 */
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

/*!
 * 内存比较
 * @param buf1 内存指针1
 * @param buf2 内存指针2
 * @param len 内存大小
 * @return 若相等，则返回0，若内存指针1某个字节单元值更小，则返回-1，否则返回1
 */
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