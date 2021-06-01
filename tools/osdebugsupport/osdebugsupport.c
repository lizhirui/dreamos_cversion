/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-20     lizhirui     the first version
 */

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <assert.h>

#define ALIGN_UP(value,align_bound) (((value) + ((align_bound) - 1)) & (~((align_bound) - 1)))
#define ALIGN_DOWN(value,align_bound) ((value) & (~(align_bound - 1)))

FILE *elf_file,*bin_file;

#ifndef ARCH64
    typedef Elf32_Ehdr Elf_Ehdr;
    typedef Elf32_Shdr Elf_Shdr;
    typedef Elf32_Sym Elf_Sym;
#else
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Shdr Elf_Shdr;
    typedef Elf64_Sym Elf_Sym;
#endif

typedef struct os_symtab_header
{
    size_t function_table_offset;
    size_t function_table_num;
    size_t object_table_offset;
    size_t object_table_num;
    size_t general_symbol_table_offset;
    size_t general_symbol_table_num;
    size_t string_table_offset;
    size_t string_table_size;
}os_symtab_header;

typedef struct os_symtab_item
{
    size_t name_offset;
    size_t address;
    size_t size;
}os_symtab_item;

int os_symtab_item_cmpfunc(const void *a,const void *b)
{
    return ((os_symtab_item *)a) -> address - ((os_symtab_item *)b) -> address;
}

void generate_file()
{
    Elf_Ehdr ehdr;
    const Elf_Shdr *shdr;
    const char *shstr_table;
    const char *str_table;

    fseek(elf_file,0,SEEK_SET);
    fread(&ehdr,sizeof(ehdr),1,elf_file);

    size_t count = ehdr.e_shnum;
    fseek(elf_file,ehdr.e_shoff,SEEK_SET);
    shdr = (Elf_Shdr *)malloc(sizeof(shdr[0]) * count);
    sizeof(shdr != NULL);
    fread((void *)shdr,sizeof(shdr[0]),count,elf_file);
    
    fseek(elf_file,shdr[ehdr.e_shstrndx].sh_offset,SEEK_SET);
    shstr_table = (const char *)malloc(shdr[ehdr.e_shstrndx].sh_size);
    assert(shstr_table != NULL);
    fread((void *)shstr_table,1,shdr[ehdr.e_shstrndx].sh_size,elf_file);

    size_t i;
    size_t symtab_index = -1;
    size_t osdebug_index = -1;
    size_t file_start_offset = 0;
    //printf("%zu\n",count);

    for(i = 0;i < count;i++)
    {
        //printf("%s,%p,%zu\n",&shstr_table[shdr[i].sh_name],(void *)(size_t)shdr[i].sh_offset,(size_t)shdr[i].sh_size);

        if(shdr[i].sh_flags & SHF_ALLOC)
        {
            if((shdr[i].sh_addr < file_start_offset) || (file_start_offset == 0))
            {
                file_start_offset = shdr[i].sh_addr;
            }
        }

        if(strcmp(&shstr_table[shdr[i].sh_name],".symtab") == 0)
        {
            symtab_index = i;
        }
        else if(strcmp(&shstr_table[shdr[i].sh_name],".strtab") == 0)
        {
            str_table = malloc(shdr[i].sh_size);
            assert(str_table != NULL);
            fseek(elf_file,shdr[i].sh_offset,SEEK_SET);
            fread((void *)str_table,shdr[i].sh_size,1,elf_file);
        }
        else if(strcmp(&shstr_table[shdr[i].sh_name],".osdebug") == 0)
        {
            osdebug_index = i;
        }
    }

    if(symtab_index == -1)
    {
        printf("can't find .symtab section\n");
        exit(7);
    }

    if(osdebug_index == -1)
    {
        printf("can't find .osdebug section\n");
        exit(8);
    }

    if(file_start_offset == 0)
    {
        printf("file_start_offset is 0\n");
        exit(9);
    }

    printf("file_start_offset: %p\n",(void *)file_start_offset);

    const Elf_Sym *sym_table = (const Elf_Sym *)malloc(shdr[symtab_index].sh_size);
    size_t sym_num = shdr[symtab_index].sh_size / sizeof(sym_table[0]);
    assert(sym_table != NULL);
    fseek(elf_file,shdr[symtab_index].sh_offset,SEEK_SET);
    fread((void *)sym_table,shdr[symtab_index].sh_size,1,elf_file);
    size_t function_symbol_num = 0;
    size_t object_symbol_num = 0;
    size_t general_symbol_num = 0;
    size_t string_table_size = 0;
    //printf("sym_num = %zu\n",sym_num);

    for(i = 0;i < sym_num;i++)
    {
        if(sym_table[i].st_info && sym_table[i].st_name && sym_table[i].st_value)
        {
            //printf("%d,%s,%d\n",sym_table[j].st_name,&str_table[sym_table[j].st_name],sym_table[j].st_info);
            size_t symbol_type = sym_table[i].st_info & 0x0f;
            size_t valid = 0;

            switch(symbol_type)
            {
                case STT_FUNC:
                    function_symbol_num++;
                    valid = 1;
                    break;

                case STT_OBJECT:
                    object_symbol_num++;
                    valid = 1;
                    break;

                case STT_NOTYPE:
                    general_symbol_num++;
                    valid = 1;
                    break;
            }

            if(valid)
            {
                string_table_size += strlen(&str_table[sym_table[i].st_name]) + 1;
            }
        }
    }

    size_t align_bound = sizeof(size_t);

    printf("align_bound: %zu\n",align_bound);
    printf("function symbol num: %zu\n",function_symbol_num);
    printf("object symbol num: %zu\n",object_symbol_num);
    printf("general symbol num: %zu\n",general_symbol_num);
    printf("string table size: %zu Byte(s)\n",string_table_size);

    size_t need_space = ALIGN_UP(sizeof(os_symtab_header),align_bound) + ALIGN_UP(function_symbol_num * sizeof(os_symtab_item),align_bound) 
        + ALIGN_UP(object_symbol_num * sizeof(os_symtab_item),align_bound) + ALIGN_UP(general_symbol_num * sizeof(os_symtab_item),align_bound)
        + ALIGN_UP(string_table_size * sizeof(char),align_bound);

    printf("need space: %zu Byte(s)\n",need_space);
    printf(".osdebug section space: %zu Byte(s)\n",(size_t)shdr[osdebug_index].sh_size);

    if(shdr[osdebug_index].sh_size < need_space)
    {
        printf(".osdebug section is too small\n");
        exit(10);
    }

    os_symtab_header header;
    os_symtab_item *function_table = NULL;
    os_symtab_item *object_table = NULL;
    os_symtab_item *general_table = NULL;
    char *string_table = NULL;

    if(function_symbol_num > 0)
    {
        function_table = malloc(sizeof(os_symtab_item) * function_symbol_num);
        assert(function_table != NULL);
    }
    
    if(object_symbol_num > 0)
    {
        object_table = malloc(sizeof(os_symtab_item) * object_symbol_num);
        assert(object_table != NULL);
    }

    if(general_symbol_num > 0)
    {
        general_table = malloc(sizeof(os_symtab_item) * general_symbol_num);
        assert(general_table != NULL);
    }

    if(string_table_size > 0)
    {
        string_table = malloc(string_table_size);
        assert(string_table != NULL);
    }

    size_t function_symbol_ptr = 0;
    size_t object_symbol_ptr = 0;
    size_t general_symbol_ptr = 0;
    size_t string_table_ptr = 0;

    for(i = 0;i < sym_num;i++)
    {
        if(sym_table[i].st_info && sym_table[i].st_name && sym_table[i].st_value)
        {
            size_t symbol_type = sym_table[i].st_info & 0x0f;
            size_t valid = 0;

            switch(symbol_type)
            {
                case STT_FUNC:
                case STT_OBJECT:
                case STT_NOTYPE:
                    valid = 1;
                    break;
            }

            if(valid)
            {
                os_symtab_item item;
                item.name_offset = string_table_ptr;
                item.address = sym_table[i].st_value;
                item.size = sym_table[i].st_size;
                strcpy(string_table + string_table_ptr,&str_table[sym_table[i].st_name]);
                string_table_ptr += strlen(&str_table[sym_table[i].st_name]) + 1;

                switch(symbol_type)
                {
                    case STT_FUNC:
                        function_table[function_symbol_ptr++] = item;
                        //printf("%d,%s: %p\n",i,&str_table[sym_table[i].st_name],(void *)item.address);
                        break;

                    case STT_OBJECT:
                        object_table[object_symbol_ptr++] = item;
                        break;

                    case STT_NOTYPE:
                        general_table[general_symbol_ptr++] = item;
                        break;
                }
            }
        }
    }

    qsort(function_table,function_symbol_num,sizeof(os_symtab_item),os_symtab_item_cmpfunc);
    qsort(object_table,object_symbol_num,sizeof(os_symtab_item),os_symtab_item_cmpfunc);
    qsort(general_table,general_symbol_num,sizeof(os_symtab_item),os_symtab_item_cmpfunc);

    /*for(i = 0;i < function_symbol_num;i++)
    {
        printf("%d,%s: %p\n",i,&string_table[function_table[i].name_offset],(void *)function_table[i].address);
    }*/

    header.function_table_offset = ALIGN_UP(sizeof(os_symtab_header),align_bound);
    header.object_table_offset = header.function_table_offset + ALIGN_UP(function_symbol_num * sizeof(os_symtab_item),align_bound);
    header.general_symbol_table_offset = header.object_table_offset + ALIGN_UP(object_symbol_num * sizeof(os_symtab_item),align_bound);
    header.string_table_offset = header.general_symbol_table_offset + ALIGN_UP(general_symbol_num * sizeof(os_symtab_item),align_bound);
    header.function_table_num = function_symbol_num;
    header.object_table_num = object_symbol_num;
    header.general_symbol_table_num = general_symbol_num;
    header.string_table_size = string_table_size;
    size_t osdebug_offset = shdr[osdebug_index].sh_addr - file_start_offset;
    printf(".osdebug in file is from %p\n",(void *)osdebug_offset);

    fseek(bin_file,osdebug_offset,SEEK_SET);
    fwrite((const void *)&header,sizeof(os_symtab_header),1,bin_file);

    fseek(bin_file,osdebug_offset + header.function_table_offset,SEEK_SET);
    fwrite((const void *)function_table,sizeof(os_symtab_item),header.function_table_num,bin_file);

    fseek(bin_file,osdebug_offset + header.object_table_offset,SEEK_SET);
    fwrite((const void *)object_table,sizeof(os_symtab_item),header.object_table_num,bin_file);

    fseek(bin_file,osdebug_offset + header.general_symbol_table_offset,SEEK_SET);
    fwrite((const void *)general_table,sizeof(os_symtab_item),header.general_symbol_table_num,bin_file);

    fseek(bin_file,osdebug_offset + header.string_table_offset,SEEK_SET);
    fwrite((const void *)string_table,sizeof(char),header.string_table_size,bin_file);
}

int main(int argc,char **argv)
{
    if(argc != 3)
    {
        printf("usage: %s elffile binaryfile\n",argv[0]);
        return 1;
    }

    char *elf_file_path = argv[1];
    char *bin_file_path = argv[2];
    elf_file = fopen(elf_file_path,"rb");
    
    if(elf_file == NULL)
    {
        printf("elf file %s can't be open\n",elf_file_path);
        return 2;
    }

    bin_file = fopen(bin_file_path,"rb+");

    if(elf_file == NULL)
    {
        printf("binary file %s can't be open\n",bin_file_path);
        return 3;
    }

    char buf[20];

    memset(buf,0,sizeof(buf));

    if(fread(buf,1,5,elf_file) != 5)
    {
        printf("elf file read fail\n");
        return 4;
    }

    if((buf[0] != 0x7f) || (buf[1] != 'E') || (buf[2] != 'L') || (buf[3] != 'F')) 
    {
        printf("elf file is error\n");
        return 5;
    }

    #ifndef ARCH64
        const char *errstr = "32-bit";
        if(buf[4] == 1)
    #else
        const char *errstr = "64-bit";
        if(buf[4] == 2)
    #endif
    {
        generate_file();
        printf("generate finished\n");
        fclose(bin_file);
        fclose(elf_file);
    }
    else
    {
        printf("elf file is not %s\n",errstr);
        return 6;
    }

    return 0;
}