/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-06     lizhirui     the first version
 * 2021-07-08     lizhirui     add getpid and getppid syscall
 */

// @formatter:off
#include <dreamos.h>

static os_syscall_handler_t syscall_handler_table[SYSCALL_MAX_NUM] = 
{
    [__NR_clone] = (os_syscall_handler_t)os_syscall_clone,
    [__NR_execve] = (os_syscall_handler_t)os_syscall_execve,
    [__NR_write] = (os_syscall_handler_t)os_syscall_write,
    [__NR_exit] = (os_syscall_handler_t)os_syscall_exit,
    [__NR_getpid] = (os_syscall_handler_t)os_syscall_getpid,
    [__NR_getppid] = (os_syscall_handler_t)os_syscall_getppid
};

void arch_syscall_handler(struct TrapFrame *regs)
{
    OS_ANNOTATION_NEED_TASK_CONTEXT();
    os_size_t syscall_id = regs -> a7;

    if(syscall_id > SYSCALL_MAX_NUM)
    {
        regs -> a0 = (os_size_t)-OS_ERR_ENOSYS;
        return;
    }
    
    if(!syscall_handler_table[syscall_id])
    {
        regs -> a0 = (os_size_t)-OS_ERR_ENOSYS;
        return;
    }

    regs -> a0 = (os_size_t)syscall_handler_table[syscall_id](regs,regs -> a0,regs -> a1,regs -> a2,regs -> a3,regs -> a4,regs -> a5);
}