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

extern size_t __global_pointer$;

void arch_task_stack_frame_init(task_t *task)
{
    task -> sp -= sizeof(struct TrapFrame);
    struct TrapFrame *frame = (struct TrapFrame *)task -> sp;
    size_t i;

    for(i = 0;i < sizeof(struct TrapFrame) / sizeof(size_t);i++)
    {
        ((size_t *)frame)[i] = 0xdeadbeef;
    }

    frame -> ra = (size_t)task -> exit_func;
    frame -> gp = (size_t)&__global_pointer$;
    frame -> a0 = task -> arg;
    frame -> sepc = (size_t)task -> entry;
    frame -> user_sp_exc_stack = task -> stack_addr + task -> stack_size;
    /* force to supervisor mode(SPP=1) and set SPIE and SUM to 1 */
    frame -> sstatus = 0x000401120;
}