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
#ifndef __OS_SYSCALL_H__
#define __OS_SYSCALL_H__

    /*
    * cloning flags:
    */
    #define OS_CLONE_CSIGNAL		0x000000ff	/* signal mask to be sent at exit */
    #define OS_CLONE_VM	0x00000100	/* set if VM shared between processes */
    #define OS_CLONE_FS	0x00000200	/* set if fs info shared between processes */
    #define OS_CLONE_FILES	0x00000400	/* set if open files shared between processes */
    #define OS_CLONE_SIGHAND	0x00000800	/* set if signal handlers and blocked signals shared */
    #define OS_CLONE_PIDFD	0x00001000	/* set if a pidfd should be placed in parent */
    #define OS_CLONE_PTRACE	0x00002000	/* set if we want to let tracing continue on the child too */
    #define OS_CLONE_VFORK	0x00004000	/* set if the parent wants the child to wake it up on mm_release */
    #define OS_CLONE_PARENT	0x00008000	/* set if we want to have the same parent as the cloner */
    #define OS_CLONE_THREAD	0x00010000	/* Same thread group? */
    #define OS_CLONE_NEWNS	0x00020000	/* New mount namespace group */
    #define OS_CLONE_SYSVSEM	0x00040000	/* share system V SEM_UNDO semantics */
    #define OS_CLONE_SETTLS	0x00080000	/* create a new TLS for the child */
    #define OS_CLONE_PARENT_SETTID	0x00100000	/* set the TID in the parent */
    #define OS_CLONE_CHILD_CLEARTID	0x00200000	/* clear the TID in the child */
    #define OS_CLONE_DETACHED		0x00400000	/* Unused, ignored */
    #define OS_CLONE_UNTRACED		0x00800000	/* set if the tracing process can't force CLONE_PTRACE on this clone */
    #define OS_CLONE_CHILD_SETTID	0x01000000	/* set the TID in the child */
    #define OS_CLONE_NEWCGROUP		0x02000000	/* New cgroup namespace */
    #define OS_CLONE_NEWUTS		0x04000000	/* New utsname namespace */
    #define OS_CLONE_NEWIPC		0x08000000	/* New ipc namespace */
    #define OS_CLONE_NEWUSER		0x10000000	/* New user namespace */
    #define OS_CLONE_NEWPID		0x20000000	/* New pid namespace */
    #define OS_CLONE_NEWNET		0x40000000	/* New network namespace */
    #define OS_CLONE_IO		0x80000000	/* Clone io context */

    #ifndef __ASSEMBLY__
        #include <dreamos.h>

        typedef os_ssize_t (*os_syscall_handler_t)(struct TrapFrame *regs,os_size_t arg0,os_size_t arg1,os_size_t arg2,os_size_t arg3,os_size_t arg4,os_size_t arg5);

        os_ssize_t os_syscall_clone(struct TrapFrame *regs,os_size_t clone_flags,os_size_t newsp,os_size_t parent_tidptr,os_size_t child_tidptr,os_size_t tls);
        os_ssize_t os_syscall_execve();
        os_ssize_t os_syscall_write();
        os_ssize_t os_syscall_exit();
        os_ssize_t os_syscall_getpid();
        os_ssize_t os_syscall_getppid();
    #endif

#endif