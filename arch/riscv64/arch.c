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

extern os_size_t __global_pointer$;

//用于arch实现初始化任务栈帧
void arch_task_stack_frame_init(os_task_t *task)
{
    task -> sp -= sizeof(struct TrapFrame);
    struct TrapFrame *frame = (struct TrapFrame *)task -> sp;
    os_size_t i;

    for(i = 0;i < sizeof(struct TrapFrame) / sizeof(os_size_t);i++)
    {
        ((os_size_t *)frame)[i] = 0xdeadbeef;
    }

    frame -> ra = (os_size_t)task -> exit_func;
    frame -> gp = (os_size_t)&__global_pointer$;
    frame -> a0 = task -> arg;
    frame -> sepc = (os_size_t)task -> entry;
    frame -> user_sp = task -> stack_addr + task -> stack_size;
    /* force to supervisor mode(SPP=1) and set SPIE and SUM to 1 */
    frame -> sstatus = 0x000401120;
}