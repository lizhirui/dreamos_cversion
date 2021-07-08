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

extern os_size_t __global_pointer$;

void syscall_exit();

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

void arch_task_clone_stack_frame_init(struct TrapFrame *regs,os_task_t *task,os_size_t new_sp)
{
    struct TrapFrame *src_frame = (struct TrapFrame *)os_task_get_current_task() -> sp;
    struct TrapFrame *dst_frame = (struct TrapFrame *)(task -> sp = (task -> stack_addr + task -> stack_size - sizeof(struct TrapFrame) * 2));

    os_memcpy(&dst_frame[1],src_frame,sizeof(struct TrapFrame));
    dst_frame[1].a0 = task -> pid;
    dst_frame[1].user_sp = new_sp;
    dst_frame[0].user_sp = task -> stack_addr + task -> stack_size - sizeof(struct TrapFrame);
    dst_frame[0].gp = (os_size_t)&__global_pointer$;
    dst_frame[0].sepc = (os_size_t)syscall_exit;
    /* force to supervisor mode(SPP=1) and set SPIE and SUM to 1 */
    dst_frame[0].sstatus = 0x000401120;
}

void arch_task_execve_stack_frame_init(struct TrapFrame *regs,os_size_t entry)
{
    os_size_t sstatus = regs -> sstatus;
    os_memset(regs,0,sizeof(struct TrapFrame));
    regs -> sepc = entry;
    regs -> sstatus = sstatus;
}