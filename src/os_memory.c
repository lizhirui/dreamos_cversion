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

//内核内存系统初始化函数
void os_memory_init()
{
    phypage_init();
}