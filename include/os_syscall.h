#ifndef __OS_SYSCALL_H__
#define __OS_SYSCALL_H__

    #include <dreamos.h>

    os_ssize_t os_syscall_clone();
    os_ssize_t os_syscall_execve();
    os_ssize_t os_syscall_write();
    os_ssize_t os_syscall_exit();

#endif