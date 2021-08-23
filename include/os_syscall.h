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

        os_ssize_t os_syscall_getcwd(struct TrapFrame *regs,os_size_t buf,os_size_t size);
        os_ssize_t os_syscall_pipe2(struct TrapFrame *regs,os_size_t *fd);
        os_ssize_t os_syscall_dup(struct TrapFrame *regs,os_size_t fd);
        os_ssize_t os_syscall_dup3(struct TrapFrame *regs,os_size_t old,os_size_t new);
        os_ssize_t os_syscall_chdir(struct TrapFrame *regs,os_size_t path);
        os_ssize_t os_syscall_openat(struct TrapFrame *regs,os_size_t fd,os_size_t filename,os_size_t flags,os_size_t mode);
        os_ssize_t os_syscall_close(struct TrapFrame *regs,os_size_t fd);
        os_ssize_t os_syscall_getdents64(struct TrapFrame *regs,os_size_t fd,os_size_t buf,os_size_t len);
        os_ssize_t os_syscall_read(struct TrapFrame *regs,os_size_t fd,os_size_t buf,os_size_t count);
        os_ssize_t os_syscall_write(struct TrapFrame *regs,os_size_t fdid,os_size_t buf,os_size_t count);
        os_ssize_t os_syscall_linkat(struct TrapFrame *regs,os_size_t olddirfd,os_size_t oldpath,os_size_t newdirfd,os_size_t newpath,os_size_t flags);
        os_ssize_t os_syscall_unlinkat(struct TrapFrame *regs,os_size_t dirfd,os_size_t path,os_size_t flags);
        os_ssize_t os_syscall_mkdirat(struct TrapFrame *regs,os_size_t dirfd,os_size_t path,os_size_t mode);
        os_ssize_t os_syscall_umount2(struct TrapFrame *regs,os_size_t special,os_size_t flag);
        os_ssize_t os_syscall_mount(struct TrapFrame *regs,os_size_t special,os_size_t dir,os_size_t fstype,os_size_t flags,os_size_t data);
        os_ssize_t os_syscall_fstat(struct TrapFrame *regs,os_size_t fd,os_size_t kst);
        os_ssize_t os_syscall_clone(struct TrapFrame *regs,os_size_t clone_flags,os_size_t newsp,os_size_t parent_tidptr,os_size_t child_tidptr,os_size_t tls);
        os_ssize_t os_syscall_execve(struct TrapFrame *regs,os_size_t filename,os_size_t argv,os_size_t argc);
        os_ssize_t os_syscall_wait4(struct TrapFrame *regs,os_size_t pid,os_size_t status,os_size_t options);
        os_ssize_t os_syscall_exit(struct TrapFrame *regs,os_ssize_t ec);
        os_ssize_t os_syscall_getpid(struct TrapFrame *regs);
        os_ssize_t os_syscall_getppid(struct TrapFrame *regs);
        os_ssize_t os_syscall_brk(struct TrapFrame *regs,os_size_t brk);
        os_ssize_t os_syscall_munmap(struct TrapFrame *regs,os_size_t start,os_size_t len);
        os_ssize_t os_syscall_mmap(struct TrapFrame *regs,os_size_t start,os_size_t len,os_size_t prot,os_size_t flags,os_size_t fd,os_size_t off);
        os_ssize_t os_syscall_times(struct TrapFrame *regs,os_size_t tms);
        os_ssize_t os_syscall_uname(struct TrapFrame *regs,os_size_t uts);
        os_ssize_t os_syscall_sched_yield(struct TrapFrame *regs);
        os_ssize_t os_syscall_gettimeofday(struct TrapFrame *regs,os_size_t ts);
        os_size_t os_syscall_nanosleep(struct TrapFrame *regs,os_size_t req,os_size_t rem);
    #endif

#endif