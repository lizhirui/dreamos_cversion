/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     lizhirui     the first version
 */

    .option norelax
    .section .text
    .global sync
sync:
    fence
    fence.i
    ret

    .global sync_data
sync_data:
    fence
    ret

sync_instruction:
    fence.i
    ret