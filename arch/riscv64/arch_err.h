/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-06     lizhirui     the first version
 */

#ifndef __ARCH_ERR_H__
#define __ARCH_ERR_H__

    #define	OS_ERR_EDEADLK		        35	/* Resource deadlock would occur */
    #define	OS_ERR_ENAMETOOLONG	        36	/* File name too long */
    #define	OS_ERR_ENOLCK		        37	/* No record locks available */

    /*
    * This error code is special: arch syscall entry code will return
    * -ENOSYS if users try to call a syscall that doesn't exist.  To keep
    * failures of syscalls that really do exist distinguishable from
    * failures due to attempts to use a nonexistent syscall, syscall
    * implementations should refrain from returning -ENOSYS.
    */
    #define	OS_ERR_ENOSYS		        38	/* Invalid system call number */

    #define	OS_ERR_ENOTEMPTY	        39	/* Directory not empty */
    #define	OS_ERR_ELOOP		        40	/* Too many symbolic links encountered */
    #define	OS_ERR_EWOULDBLOCK	        OS_ERR_EAGAIN	/* Operation would block */
    #define	OS_ERR_ENOMSG		        42	/* No message of desired type */
    #define	OS_ERR_EIDRM		        43	/* Identifier removed */
    #define	OS_ERR_ECHRNG		        44	/* Channel number out of range */
    #define	OS_ERR_EL2NSYNC	            45	/* Level 2 not synchronized */
    #define	OS_ERR_EL3HLT		        46	/* Level 3 halted */
    #define	EL3RST		                47	/* Level 3 reset */
    #define	OS_ERR_ELNRNG		        48	/* Link number out of range */
    #define	OS_ERR_EUNATCH		        49	/* Protocol driver not attached */
    #define	OS_ERR_ENOCSI		        50	/* No CSI structure available */
    #define	OS_ERR_EL2HLT		        51	/* Level 2 halted */
    #define	OS_ERR_EBADE		        52	/* Invalid exchange */
    #define	OS_ERR_EBADR		        53	/* Invalid request descriptor */
    #define	OS_ERR_EXFULL		        54	/* Exchange full */
    #define	OS_ERR_ENOANO		        55	/* No anode */
    #define	OS_ERR_EBADRQC		        56	/* Invalid request code */
    #define	OS_ERR_EBADSLT		        57	/* Invalid slot */

    #define	OS_ERR_EDEADLOCK	        OS_ERR_EDEADLK

    #define	OS_ERR_EBFONT		        59	/* Bad font file format */
    #define	OS_ERR_ENOSTR		        60	/* Device not a stream */
    #define	OS_ERR_ENODATA		        61	/* No data available */
    #define	OS_ERR_ETIME		        62	/* Timer expired */
    #define	OS_ERR_ENOSR		        63	/* Out of streams resources */
    #define	OS_ERR_ENONET		        64	/* Machine is not on the network */
    #define	OS_ERR_ENOPKG		        65	/* Package not installed */
    #define	OS_ERR_EREMOTE		        66	/* Object is remote */
    #define	OS_ERR_ENOLINK		        67	/* Link has been severed */
    #define	OS_ERR_EADV		            68	/* Advertise error */
    #define	OS_ERR_ESRMNT		        69	/* Srmount error */
    #define	OS_ERR_ECOMM		        70	/* Communication error on send */
    #define	OS_ERR_EPROTO		        71	/* Protocol error */
    #define	OS_ERR_EMULTIHOP	        72	/* Multihop attempted */
    #define	OS_ERR_EDOTDOT		        73	/* RFS specific error */
    #define	OS_ERR_EBADMSG		        74	/* Not a data message */
    #define	OS_ERR_EOVERFLOW	        75	/* Value too large for defined data type */
    #define	OS_ERR_ENOTUNIQ	            76	/* Name not unique on network */
    #define	OS_ERR_EBADFD		        77	/* File descriptor in bad state */
    #define	OS_ERR_EREMCHG		        78	/* Remote address changed */
    #define	OS_ERR_ELIBACC		        79	/* Can not access a needed shared library */
    #define	OS_ERR_ELIBBAD		        80	/* Accessing a corrupted shared library */
    #define	OS_ERR_ELIBSCN		        81	/* .lib section in a.out corrupted */
    #define	OS_ERR_ELIBMAX		        82	/* Attempting to link in too many shared libraries */
    #define	OS_ERR_ELIBEXEC	            83	/* Cannot exec a shared library directly */
    #define	OS_ERR_EILSEQ		        84	/* Illegal byte sequence */
    #define	OS_ERR_ERESTART	            85	/* Interrupted system call should be restarted */
    #define	OS_ERR_ESTRPIPE	            86	/* Streams pipe error */
    #define	OS_ERR_EUSERS		        87	/* Too many users */
    #define	OS_ERR_ENOTSOCK	            88	/* Socket operation on non-socket */
    #define	OS_ERR_EDESTADDRREQ	        89	/* Destination address required */
    #define	OS_ERR_EMSGSIZE	            90	/* Message too long */
    #define	OS_ERR_EPROTOTYPE	        91	/* Protocol wrong type for socket */
    #define	OS_ERR_ENOPROTOOPT	        92	/* Protocol not available */
    #define	OS_ERR_EPROTONOSUPPORT	    93	/* Protocol not supported */
    #define	OS_ERR_ESOCKTNOSUPPORT	    94	/* Socket type not supported */
    #define	OS_ERR_EOPNOTSUPP	        95	/* Operation not supported on transport endpoint */
    #define	OS_ERR_EPFNOSUPPORT	        96	/* Protocol family not supported */
    #define	OS_ERR_EAFNOSUPPORT	        97	/* Address family not supported by protocol */
    #define	OS_ERR_EADDRINUSE	        98	/* Address already in use */
    #define	OS_ERR_EADDRNOTAVAIL    	99	/* Cannot assign requested address */
    #define	OS_ERR_ENETDOWN	            100	/* Network is down */
    #define	OS_ERR_ENETUNREACH      	101	/* Network is unreachable */
    #define	OS_ERR_ENETRESET	        102	/* Network dropped connection because of reset */
    #define	OS_ERR_ECONNABORTED     	103	/* Software caused connection abort */
    #define	OS_ERR_ECONNRESET	        104	/* Connection reset by peer */
    #define	OS_ERR_ENOBUFS		        105	/* No buffer space available */
    #define	OS_ERR_EISCONN		        106	/* Transport endpoint is already connected */
    #define	OS_ERR_ENOTCONN	            107	/* Transport endpoint is not connected */
    #define	OS_ERR_ESHUTDOWN        	108	/* Cannot send after transport endpoint shutdown */
    #define	OS_ERR_ETOOMANYREFS     	109	/* Too many references: cannot splice */
    #define	OS_ERR_ETIMEDOUT	        110	/* Connection timed out */
    #define	OS_ERR_ECONNREFUSED     	111	/* Connection refused */
    #define	OS_ERR_EHOSTDOWN	        112	/* Host is down */
    #define	OS_ERR_EHOSTUNREACH	        113	/* No route to host */
    #define	OS_ERR_EALREADY	            114	/* Operation already in progress */
    #define	OS_ERR_EINPROGRESS      	115	/* Operation now in progress */
    #define	OS_ERR_ESTALE		        116	/* Stale file handle */
    #define	OS_ERR_EUCLEAN		        117	/* Structure needs cleaning */
    #define	OS_ERR_ENOTNAM		        118	/* Not a XENIX named type file */
    #define	OS_ERR_ENAVAIL		        119	/* No XENIX semaphores available */
    #define	OS_ERR_EISNAM		        120	/* Is a named type file */
    #define	OS_ERR_EREMOTEIO	        121	/* Remote I/O error */
    #define	OS_ERR_EDQUOT		        122	/* Quota exceeded */

    #define	OS_ERR_ENOMEDIUM	        123	/* No medium found */
    #define	OS_ERR_EMEDIUMTYPE	        124	/* Wrong medium type */
    #define	OS_ERR_ECANCELED	        125	/* Operation Canceled */
    #define	OS_ERR_ENOKEY		        126	/* Required key not available */
    #define	OS_ERR_EKEYEXPIRED	        127	/* Key has expired */
    #define	OS_ERR_EKEYREVOKED	        128	/* Key has been revoked */
    #define	OS_ERR_EKEYREJECTED	        129	/* Key was rejected by service */

    /* for robust mutexes */
    #define	OS_ERR_EOWNERDEAD	        130	/* Owner died */
    #define	OS_ERR_ENOTRECOVERABLE	    131	/* State not recoverable */

    #define OS_ERR_ERFKILL		        132	/* Operation not possible due to RF-kill */

    #define OS_ERR_EHWPOISON	        133	/* Memory page has hardware error */

#endif