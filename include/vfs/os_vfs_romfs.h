/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-06     lizhirui     the first version
 */

#ifndef __OS_VFS_ROMFS_H__
#define __OS_VFS_ROMFS_H__

    #include <dreamos.h>

    typedef struct os_vfs_romfs_dirent
    {
        char *name;
        os_size_t size;
        const os_uint8_t *data;
        os_size_t type;
    }os_vfs_romfs_dirent_t,*os_vfs_romfs_dirent_p;

    os_err_t os_vfs_romfs_init();

#endif