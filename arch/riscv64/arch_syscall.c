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
    [__NR_getcwd] = (os_syscall_handler_t)os_syscall_getcwd,
    [__NR_pipe2] = (os_syscall_handler_t)os_syscall_pipe2,
    [__NR_dup] = (os_syscall_handler_t)os_syscall_dup,
    [__NR_dup3] = (os_syscall_handler_t)os_syscall_dup3,
    [__NR_chdir] = (os_syscall_handler_t)os_syscall_chdir,
    [__NR_openat] = (os_syscall_handler_t)os_syscall_openat,
    [__NR_close] = (os_syscall_handler_t)os_syscall_close,
    [__NR_getdents64] = (os_syscall_handler_t)os_syscall_getdents64,
    [__NR_read] = (os_syscall_handler_t)os_syscall_read,
    [__NR_write] = (os_syscall_handler_t)os_syscall_write,
    [__NR_linkat] = (os_syscall_handler_t)os_syscall_linkat,
    [__NR_unlinkat] = (os_syscall_handler_t)os_syscall_unlinkat,
    [__NR_mkdirat] = (os_syscall_handler_t)os_syscall_mkdirat,
    [__NR_umount2] = (os_syscall_handler_t)os_syscall_umount2,
    [__NR_mount] = (os_syscall_handler_t)os_syscall_mount,
    [__NR_fstat] = (os_syscall_handler_t)os_syscall_fstat,
    [__NR_clone] = (os_syscall_handler_t)os_syscall_clone,
    [__NR_execve] = (os_syscall_handler_t)os_syscall_execve,
    [__NR_wait4] = (os_syscall_handler_t)os_syscall_wait4,
    [__NR_exit] = (os_syscall_handler_t)os_syscall_exit,
    [__NR_getpid] = (os_syscall_handler_t)os_syscall_getpid,
    [__NR_getppid] = (os_syscall_handler_t)os_syscall_getppid,
    [__NR_brk] = (os_syscall_handler_t)os_syscall_brk,
    [__NR_munmap] = (os_syscall_handler_t)os_syscall_munmap,
    [__NR_mmap] = (os_syscall_handler_t)os_syscall_mmap,
    [__NR_times] = (os_syscall_handler_t)os_syscall_times,
    [__NR_uname] = (os_syscall_handler_t)os_syscall_uname,
    [__NR_sched_yield] = (os_syscall_handler_t)os_syscall_sched_yield,
    [__NR_gettimeofday] = (os_syscall_handler_t)os_syscall_gettimeofday,
    [__NR_nanosleep] = (os_syscall_handler_t)os_syscall_nanosleep
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
    regs -> sepc += 4;
}