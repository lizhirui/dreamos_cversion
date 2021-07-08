/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-04     lizhirui     the first version
 * 2021-07-06     lizhirui     add arch error
 */

// @formatter:off
#ifndef __OS_ERR_H__
#define __OS_ERR_H__

    #include <dreamos.h>

    typedef os_ssize_t os_err_t;

    #define OS_ERR_OK           0
    #define	OS_ERR_EPERM		1	/* Operation not permitted */
    #define	OS_ERR_ENOENT		2	/* No such file or directory */
    #define	OS_ERR_ESRCH		3	/* No such process */
    #define	OS_ERR_EINTR		4	/* Interrupted system call */
    #define	OS_ERR_EIO		    5	/* I/O error */
    #define	OS_ERR_ENXIO		6	/* No such device or address */
    #define	OS_ERR_E2BIG		7	/* Argument list too long */
    #define	OS_ERR_ENOEXEC		8	/* Exec format error */
    #define	OS_ERR_EBADF		9	/* Bad file number */
    #define	OS_ERR_ECHILD		10	/* No child processes */
    #define	OS_ERR_EAGAIN		11	/* Try again */
    #define	OS_ERR_ENOMEM		12	/* Out of memory */
    #define	OS_ERR_EACCES		13	/* Permission denied */
    #define	OS_ERR_EFAULT		14	/* Bad address */
    #define	OS_ERR_ENOTBLK		15	/* Block device required */
    #define	OS_ERR_EBUSY		16	/* Device or resource busy */
    #define	OS_ERR_EEXIST		17	/* File exists */
    #define	OS_ERR_EXDEV		18	/* Cross-device link */
    #define	OS_ERR_ENODEV		19	/* No such device */
    #define	OS_ERR_ENOTDIR		20	/* Not a directory */
    #define	OS_ERR_EISDIR		21	/* Is a directory */
    #define	OS_ERR_EINVAL		22	/* Invalid argument */
    #define	OS_ERR_ENFILE		23	/* File table overflow */
    #define	OS_ERR_EMFILE		24	/* Too many open files */
    #define	OS_ERR_ENOTTY		25	/* Not a typewriter */
    #define	OS_ERR_ETXTBSY		26	/* Text file busy */
    #define	OS_ERR_EFBIG		27	/* File too large */
    #define	OS_ERR_ENOSPC		28	/* No space left on device */
    #define	OS_ERR_ESPIPE		29	/* Illegal seek */
    #define	OS_ERR_EROFS		30	/* Read-only file system */
    #define	OS_ERR_EMLINK		31	/* Too many links */
    #define	OS_ERR_EPIPE		32	/* Broken pipe */
    #define	OS_ERR_EDOM		    33	/* Math argument out of domain of func */
    #define	OS_ERR_ERANGE		34	/* Math result not representable */
    
    #include <arch_err.h>

    #define OS_ERR_RETURN_ERROR(condition,errorcode) do{OS_BUILD_ASSERT((errorcode) <= 0);if((condition)){return (errorcode);}}while(0)
    #define OS_ERR_SET_ERROR_AND_GOTO(condition,variable,errorcode,label) do{OS_BUILD_ASSERT((errorcode) <= 0);if((condition)){variable = (errorcode);goto label;}}while(0)
    #define OS_ERR_GET_ERROR_AND_GOTO(expression,variable,label) do{if((variable = (expression)) < OS_ERR_OK){goto label;}}while(0)
    #define OS_ERR_GET_ERROR_AND_RETURN(expression) do{os_err_t err;if((err = (expression)) < OS_ERR_OK){return err;}}while(0)

#endif