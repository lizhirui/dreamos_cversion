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

static os_task_t task1,task2;
static os_waitqueue_t waitqueue;
static os_mutex_t mutex;

/*
const static unsigned char _dummy_dummy_txt[] =
{
    0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x66, 0x69, 0x6c, 0x65, 0x21, 0x0d, 0x0a,
};

const static os_vfs_romfs_dirent_t _dummy[] =
{
    {.type = OS_FILE_TYPE_REGULAR,.name = "dummy.txt",.data = _dummy_dummy_txt,.size = sizeof(_dummy_dummy_txt)},
};

const static unsigned char _dummy_txt[] =
{
    0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x66, 0x69, 0x6c, 0x65, 0x21, 0x0d, 0x0a,
};

const static os_vfs_romfs_dirent_t _root_dirent[] =
{
    {.type = OS_FILE_TYPE_DIRECTORY,.name = "dummy",.data = (os_uint8_t *)_dummy,.size = sizeof(_dummy) / sizeof(_dummy[0])},
    {.type = OS_FILE_TYPE_REGULAR,.name = "dummy.txt",.data = _dummy_txt,.size = sizeof(_dummy_txt)},
};

const static os_vfs_romfs_dirent_t romfs_root =
{
    .type = OS_FILE_TYPE_DIRECTORY,.name = "/",.data = (os_uint8_t *)_root_dirent,.size = sizeof(_root_dirent) / sizeof(_root_dirent[0])
};

static void vfs_romfs_test()
{
    os_file_fd_t root_fd;
    os_err_t ret;

    os_printf("\nlist /\n");
    OS_ASSERT((ret = os_file_open(&root_fd,"/",OS_FILE_FLAG_RDONLY | OS_FILE_FLAG_DIRECTORY)) == OS_ERR_OK);
    //os_printf("open_root_dir = %d\n",);

    os_dirent_t dirent;

    while((ret = os_file_readdir(&root_fd,&dirent,1)) > 0)
    {
        os_printf("name = %s,type = %d\n",dirent.name,dirent.type);
    }

    os_printf("os_file_readdir = %d\n",ret);
    OS_ASSERT(ret == OS_ERR_OK);
    OS_ASSERT(os_file_close(&root_fd) == OS_ERR_OK);
    //os_printf("os_file_close = %d\n",os_file_close(&root_fd));

    os_file_fd_t dummy_fd;

    os_printf("\nlist /dummy\n");
    OS_ASSERT((ret = os_file_open(&dummy_fd,"/dummy",OS_FILE_FLAG_RDONLY | OS_FILE_FLAG_DIRECTORY)) == OS_ERR_OK);
    //os_printf("open_dummy_dir = %d\n",);

    while((ret = os_file_readdir(&dummy_fd,&dirent,1)) > 0)
    {
        os_printf("name = %s,type = %d\n",dirent.name,dirent.type);
    }

    os_printf("os_file_readdir = %d\n",ret);
    OS_ASSERT(ret == OS_ERR_OK);
    OS_ASSERT(os_file_close(&dummy_fd) == OS_ERR_OK);
    //os_printf("os_file_close = %d\n",os_file_close(&dummy_fd));
    
    char buf[50];

    os_printf("\ncat /dummy.txt\n");
    //os_printf("open_dummy.txt = %d\n",os_file_open(&dummy_fd,"/dummy.txt",OS_FILE_FLAG_RDONLY));
    OS_ASSERT(os_file_open(&dummy_fd,"/dummy.txt",OS_FILE_FLAG_RDONLY) == OS_ERR_OK);

    ret = os_file_read(&dummy_fd,buf,sizeof(buf));
    os_printf("read_dummy.txt = %d\n",ret);
    OS_ASSERT(ret >= 0);
    buf[ret] = '\0';
    os_printf("content:\n%s\n",buf);
    OS_ASSERT(os_file_close(&dummy_fd) == OS_ERR_OK);
    //os_printf("os_file_close = %d\n",os_file_close(&dummy_fd));

    os_printf("\ncat /dummy/dummy.txt\n");
    os_printf("open_dummy.txt = %d\n",os_file_open(&dummy_fd,"/dummy/dummy.txt",OS_FILE_FLAG_RDONLY));

    ret = os_file_read(&dummy_fd,buf,sizeof(buf));

    os_printf("read_dummy.txt = %d\n",ret);
    OS_ASSERT(ret >= 0);
    buf[ret] = '\0';
    os_printf("content:\n%s\n",buf);
    OS_ASSERT(os_file_close(&dummy_fd) == OS_ERR_OK);
    //os_printf("os_file_close = %d\n",os_file_close(&dummy_fd));

    os_file_state_t state;

    ret = os_vfs_stat("/dummy.txt",&state);
    os_printf("\nstat /dummy.txt:%d\n",ret);
    OS_ASSERT(ret == OS_ERR_OK);
    os_printf("type = %d,size = %d\n",state.type,state.size);

    ret = os_vfs_stat("/dummy/dummy.txt",&state);
    os_printf("\nstat /dummy/dummy.txt:%d\n",ret);
    OS_ASSERT(ret == OS_ERR_OK);
    os_printf("type = %d,size = %d\n",state.type,state.size);

    os_file_fd_t dummy2_fd;

    os_printf("\ncat /dummy.txt twice time\n");
    OS_ASSERT(os_file_open(&dummy_fd,"/dummy.txt",OS_FILE_FLAG_RDONLY) == OS_ERR_OK);
    OS_ASSERT(os_file_open(&dummy2_fd,"/dummy.txt",OS_FILE_FLAG_RDONLY) == OS_ERR_OK);
    //os_printf("open_dummy.txt = %d\n",os_file_open(&dummy_fd,"/dummy.txt",OS_FILE_FLAG_RDONLY));
    //os_printf("open_dummy.txt = %d\n",os_file_open(&dummy2_fd,"/dummy.txt",OS_FILE_FLAG_RDONLY));

    ret = os_file_read(&dummy_fd,buf,sizeof(buf));

    os_printf("read_dummy.txt = %d\n",ret);
    OS_ASSERT(ret >= 0);
    buf[ret] = '\0';
    os_printf("content:\n%s\n",buf);
    ret = os_file_read(&dummy2_fd,buf,sizeof(buf));

    os_printf("read_dummy.txt = %d\n",ret);
    OS_ASSERT(ret >= 0);
    buf[ret] = '\0';
    os_printf("content:\n%s\n",buf);
    OS_ASSERT(os_file_close(&dummy2_fd) == OS_ERR_OK);
    OS_ASSERT(os_file_close(&dummy_fd) == OS_ERR_OK);
    //os_printf("os_file_close = %d\n",os_file_close(&dummy2_fd));
    //os_printf("os_file_close = %d\n",os_file_close(&dummy_fd));
}*/

os_ssize_t task1_entry(os_size_t arg)
{
    //vfs_romfs_test();

    while(1)
    {
        //vfs_romfs_test();
        //os_mutex_lock(&mutex);
        //os_printf("task1_entry\n");
        //os_waitqueue_wakeup(&waitqueue);
        //os_mutex_unlock(&mutex);
    }
}

os_ssize_t task2_entry(os_size_t arg)
{
    //vfs_romfs_test();

    while(1)
    {
        //vfs_romfs_test();
        //os_mutex_lock(&mutex);
        //os_printf("task2_entry\n");
        //os_waitqueue_wakeup(&waitqueue);
        //os_mutex_unlock(&mutex);
    }
}

static os_task_t task_user;

extern void *user_entry_code;
extern void *user_entry_code_end;

void enter_user_space(os_size_t entry);

static OS_NORETURN os_ssize_t os_task_user_entry(os_size_t arg)
{
    os_size_t start = (os_size_t)&user_entry_code;
    os_size_t end = (os_size_t)&user_entry_code_end;
    os_size_t size = ALIGN_UP(end - start,OS_MMU_PAGE_SIZE);

    os_task_p task = os_task_get_current_task();

    task -> vtable = os_memory_alloc(sizeof(os_mmu_vtable_t));
    OS_ASSERT(task -> vtable);
    OS_ASSERT(os_mmu_vtable_create(task -> vtable,OS_NULL,OS_MMU_MEMORYMAP_USER_VTABLE_START,OS_MMU_MEMORYMAP_USER_VTABLE_SIZE) == OS_ERR_OK);
    os_mmu_io_mapping_copy(task -> vtable);
    os_mmu_kernel_mapping_copy(task -> vtable);
    void *mem = os_memory_alloc(size);
    OS_ASSERT(mem);
    os_memcpy(mem,(void *)start,size);
    os_mmu_pt_prot_t prot = OS_MMU_PROT_USER;
    OS_MMU_PROT_RWX(&prot);
    OS_ASSERT(os_mmu_create_mapping(task -> vtable,arg,OS_MMU_VA_TO_PA((os_size_t)mem),size,prot) == OS_ERR_OK);

    os_mmu_switch(task -> vtable);
    enter_user_space(arg);
    while(1);
}

extern const os_vfs_romfs_dirent_t romfs_root;

os_ssize_t os_task_main_entry(os_size_t arg)
{
    os_vfs_romfs_init();

    os_printf("\nmount root filesystem\n");
    os_printf("mount = %d\n",os_vfs_mount("/","romfs",OS_NULL,OS_FILE_FLAG_RDWR,(void *)&romfs_root));

    os_mutex_init(&mutex);
    //os_task_init(&task1,MAIN_TASK_STACK_SIZE,MAIN_TASK_PRIORITY,MAIN_TASK_TICK_INIT,task1_entry,0,"task1");
    //os_task_init(&task2,MAIN_TASK_STACK_SIZE,MAIN_TASK_PRIORITY,MAIN_TASK_TICK_INIT,task2_entry,0,"task2");
    os_task_init(&task_user,MAIN_TASK_STACK_SIZE,MAIN_TASK_PRIORITY,MAIN_TASK_TICK_INIT,os_task_user_entry,OS_MMU_MEMORYMAP_USER_REAL_START,"task_user");
    os_task_startup(&task_user);
    //os_task_startup(&task1);
    //os_task_startup(&task2);
    os_waitqueue_init(&waitqueue);
    //os_waitqueue_wait(&waitqueue);

    //vfs_romfs_test();
    //vfs_romfs_test();
    //while(1);

    os_task_print_tree(os_task_get_root_task());

    while(1)
    {
        //vfs_romfs_test();
        /*os_mutex_lock(&mutex);
        os_printf("os_task_main_entry\n");
        os_mutex_unlock(&mutex);*/

        os_printf("task_main - 1s\n");
        os_printf("memory:%d/%d\n",os_get_allocated_memory(),os_get_total_memory());
        os_size_t tick = os_tick_get();

        while((os_tick_get() - tick) < TICK_PER_SECOND);
    }
}