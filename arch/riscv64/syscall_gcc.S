/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

#include "encoding.h"
#include "stackframe.h"
    .option norelax
    .section .text
    .align 2
    .global syscall_entry
    
syscall_entry:
    j syscall_entry