/*
 * Copyright lizhirui
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-08     lizhirui     the first version
 */

// @formatter:off
#ifndef __OS_ELF_H__
#define __OS_ELF_H__

    //modify from "include/uapi/linux/elf.h" from linux
    #include <dreamos.h>

    /* 32-bit ELF base types. */
    typedef os_uint32_t	os_elf_elf32_addr_t;
    typedef os_uint16_t	os_elf_elf32_half_t;
    typedef os_uint32_t	os_elf_elf32_off_t;
    typedef os_int32_t	os_elf_elf32_sword_t;
    typedef os_uint32_t	os_elf_elf32_word_t;

    /* 64-bit ELF base types. */
    typedef os_uint64_t	os_elf_elf64_addr_t;
    typedef os_uint16_t	os_elf_elf64_half_t;
    typedef os_int16_t	os_elf_elf64_shalf_t;
    typedef os_uint64_t	os_elf_elf64_off_t;
    typedef os_int32_t	os_elf_elf64_sword_t;
    typedef os_uint32_t	os_elf_elf64_word_t;
    typedef os_uint64_t	os_elf_elf64_xword_t;
    typedef os_int64_t	os_elf_elf64_sxword_t;

    /* These constants are for the segment types stored in the image headers */
    #define OS_ELF_PT_NULL    0
    #define OS_ELF_PT_LOAD    1
    #define OS_ELF_PT_DYNAMIC 2
    #define OS_ELF_PT_INTERP  3
    #define OS_ELF_PT_NOTE    4
    #define OS_ELF_PT_SHLIB   5
    #define OS_ELF_PT_PHDR    6
    #define OS_ELF_PT_TLS     7               /* Thread local storage segment */
    #define OS_ELF_PT_LOOS    0x60000000      /* OS-specific */
    #define OS_ELF_PT_HIOS    0x6fffffff      /* OS-specific */
    #define OS_ELF_PT_LOPROC  0x70000000
    #define OS_ELF_PT_HIPROC  0x7fffffff
    #define OS_ELF_PT_GNU_EH_FRAME		0x6474e550
    #define OS_ELF_PT_GNU_PROPERTY		0x6474e553

    #define OS_ELF_PT_GNU_STACK	(OS_ELF_PT_LOOS + 0x474e551)

    /*
     * Extended Numbering
     *
     * If the real number of program header table entries is larger than
     * or equal to OS_ELF_PN_XNUM(0xffff), it is set to sh_info field of the
     * section header at index 0, and OS_ELF_PN_XNUM is set to e_phnum
     * field. Otherwise, the section header at index 0 is zero
     * initialized, if it exists.
     *
     * Specifications are available in:
     *
     * - Oracle: Linker and Libraries.
     *   Part No: 817–1984–19, August 2011.
     *   https://docs.oracle.com/cd/E18752_01/pdf/817-1984.pdf
     *
     * - System V ABI AMD64 Architecture Processor Supplement
     *   Draft Version 0.99.4,
     *   January 13, 2010.
     *   http://www.cs.washington.edu/education/courses/cse351/12wi/supp-docs/abi.pdf
     */
    #define OS_ELF_PN_XNUM 0xffff

    /* These constants define the different elf file types */
    #define OS_ELF_ET_NONE   0
    #define OS_ELF_ET_REL    1
    #define OS_ELF_ET_EXEC   2
    #define OS_ELF_ET_DYN    3
    #define OS_ELF_ET_CORE   4
    #define OS_ELF_ET_LOPROC 0xff00
    #define OS_ELF_ET_HIPROC 0xffff

    /* This is the info that is needed to parse the dynamic section of the file */
    #define OS_ELF_DT_NULL		0
    #define OS_ELF_DT_NEEDED	1
    #define OS_ELF_DT_PLTRELSZ	2
    #define OS_ELF_DT_PLTGOT	3
    #define OS_ELF_DT_HASH		4
    #define OS_ELF_DT_STRTAB	5
    #define OS_ELF_DT_SYMTAB	6
    #define OS_ELF_DT_RELA		7
    #define OS_ELF_DT_RELASZ	8
    #define OS_ELF_DT_RELAENT	9
    #define OS_ELF_DT_STRSZ	10
    #define OS_ELF_DT_SYMENT	11
    #define OS_ELF_DT_INIT		12
    #define OS_ELF_DT_FINI		13
    #define OS_ELF_DT_SONAME	14
    #define OS_ELF_DT_RPATH 	15
    #define OS_ELF_DT_SYMBOLIC	16
    #define OS_ELF_DT_REL	        17
    #define OS_ELF_DT_RELSZ	18
    #define OS_ELF_DT_RELENT	19
    #define OS_ELF_DT_PLTREL	20
    #define OS_ELF_DT_DEBUG	21
    #define OS_ELF_DT_TEXTREL	22
    #define OS_ELF_DT_JMPREL	23
    #define OS_ELF_DT_ENCODING	32
    #define OS_ELF_OLD_DT_LOOS	0x60000000
    #define OS_ELF_DT_LOOS		0x6000000d
    #define OS_ELF_DT_HIOS		0x6ffff000
    #define OS_ELF_DT_VALRNGLO	0x6ffffd00
    #define OS_ELF_DT_VALRNGHI	0x6ffffdff
    #define OS_ELF_DT_ADDRRNGLO	0x6ffffe00
    #define OS_ELF_DT_ADDRRNGHI	0x6ffffeff
    #define OS_ELF_DT_VERSYM	0x6ffffff0
    #define OS_ELF_DT_RELACOUNT	0x6ffffff9
    #define OS_ELF_DT_RELCOUNT	0x6ffffffa
    #define OS_ELF_DT_FLAGS_1	0x6ffffffb
    #define OS_ELF_DT_VERDEF	0x6ffffffc
    #define	OS_ELF_DT_VERDEFNUM	0x6ffffffd
    #define OS_ELF_DT_VERNEED	0x6ffffffe
    #define	OS_ELF_DT_VERNEEDNUM	0x6fffffff
    #define OS_ELF_OLD_DT_HIOS     0x6fffffff
    #define OS_ELF_DT_LOPROC	0x70000000
    #define OS_ELF_DT_HIPROC	0x7fffffff

    /* This info is needed when parsing the symbol table */
    #define OS_ELF_STB_LOCAL  0
    #define OS_ELF_STB_GLOBAL 1
    #define OS_ELF_STB_WEAK   2

    #define OS_ELF_STT_NOTYPE  0
    #define OS_ELF_STT_OBJECT  1
    #define OS_ELF_STT_FUNC    2
    #define OS_ELF_STT_SECTION 3
    #define OS_ELF_STT_FILE    4
    #define OS_ELF_STT_COMMON  5
    #define OS_ELF_STT_TLS     6

    #define OS_ELF_ST_BIND(x)		((x) >> 4)
    #define OS_ELF_ST_TYPE(x)		(((unsigned int) x) & 0xf)
    #define OS_ELF32_ST_BIND(x)	OS_ELF_ST_BIND(x)
    #define OS_ELF32_ST_TYPE(x)	OS_ELF_ST_TYPE(x)
    #define OS_ELF64_ST_BIND(x)	OS_ELF_ST_BIND(x)
    #define OS_ELF64_ST_TYPE(x)	OS_ELF_ST_TYPE(x)

    typedef struct os_elf_dynamic
    {
        os_elf_elf32_sword_t d_tag;

        union
        {
            os_elf_elf32_sword_t	d_val;
            os_elf_elf32_addr_t	d_ptr;
        }d_un;
    }os_elf_elf32_dyn_t,*os_elf_elf32_dyn_p;

    typedef struct
    {
        os_elf_elf64_sxword_t d_tag;		/* entry tag value */

        union
        {
            os_elf_elf64_xword_t d_val;
            os_elf_elf64_addr_t d_ptr;
        }d_un;

    }os_elf_elf64_dyn_t,*os_elf_elf64_dyn_p;

    /* The following are used with relocations */
    #define OS_ELF32_R_SYM(x) ((x) >> 8)
    #define OS_ELF32_R_TYPE(x) ((x) & 0xff)

    #define OS_ELF64_R_SYM(i)			((i) >> 32)
    #define OS_ELF64_R_TYPE(i)			((i) & 0xffffffff)

    typedef struct os_elf_elf32_rel
    {
        os_elf_elf32_addr_t	r_offset;
        os_elf_elf32_word_t	r_info;
    }os_elf_elf32_rel_t,*os_elf_elf32_rel_p;

    typedef struct os_elf_elf64_rel
    {
        os_elf_elf64_addr_t r_offset;	/* Location at which to apply the action */
        os_elf_elf64_xword_t r_info;	/* index and type of relocation */
    }os_elf_elf64_rel_t,*os_elf_elf64_rel_p;

    typedef struct os_elf_elf32_rela
    {
        os_elf_elf32_addr_t	r_offset;
        os_elf_elf32_word_t	r_info;
        os_elf_elf32_sword_t	r_addend;
    }os_elf_elf32_rela_t,*os_elf_elf32_rela_p;

    typedef struct os_elf_elf64_rela
    {
        os_elf_elf64_addr_t r_offset;	/* Location at which to apply the action */
        os_elf_elf64_xword_t r_info;	/* index and type of relocation */
        os_elf_elf64_sxword_t r_addend;	/* Constant addend used to compute value */
    }os_elf_elf64_rela_t,*os_elf_elf64_rela_p;

    typedef struct os_elf_elf32_sym
    {
        os_elf_elf32_word_t	st_name;
        os_elf_elf32_addr_t	st_value;
        os_elf_elf32_word_t	st_size;
        unsigned char	st_info;
        unsigned char	st_other;
        os_elf_elf32_half_t	st_shndx;
    }os_elf_elf32_sym_t,*os_elf_elf32_sym_p;

    typedef struct os_elf_elf64_sym
    {
        os_elf_elf64_word_t st_name;		/* Symbol name, index in string tbl */
        unsigned char	st_info;	/* Type and binding attributes */
        unsigned char	st_other;	/* No defined meaning, 0 */
        os_elf_elf64_half_t st_shndx;		/* Associated section index */
        os_elf_elf64_addr_t st_value;		/* Value of the symbol */
        os_elf_elf64_xword_t st_size;		/* Associated symbol size */
    }os_elf_elf64_sym_t,*os_elf_elf64_sym_p;


    #define OS_ELF_EI_NIDENT	16

    typedef struct os_elf_elf32_hdr
    {
        unsigned char	e_ident[OS_ELF_EI_NIDENT];
        os_elf_elf32_half_t	e_type;
        os_elf_elf32_half_t	e_machine;
        os_elf_elf32_word_t	e_version;
        os_elf_elf32_addr_t	e_entry;  /* Entry point */
        os_elf_elf32_off_t	e_phoff;
        os_elf_elf32_off_t	e_shoff;
        os_elf_elf32_word_t	e_flags;
        os_elf_elf32_half_t	e_ehsize;
        os_elf_elf32_half_t	e_phentsize;
        os_elf_elf32_half_t	e_phnum;
        os_elf_elf32_half_t	e_shentsize;
        os_elf_elf32_half_t	e_shnum;
        os_elf_elf32_half_t	e_shstrndx;
    }os_elf_elf32_ehdr_t,*os_elf_elf32_ehdr_p;

    typedef struct os_elf_elf64_hdr
    {
        unsigned char	e_ident[OS_ELF_EI_NIDENT];	/* ELF "magic number" */
        os_elf_elf64_half_t e_type;
        os_elf_elf64_half_t e_machine;
        os_elf_elf64_word_t e_version;
        os_elf_elf64_addr_t e_entry;		/* Entry point virtual address */
        os_elf_elf64_off_t e_phoff;		/* Program header table file offset */
        os_elf_elf64_off_t e_shoff;		/* Section header table file offset */
        os_elf_elf64_word_t e_flags;
        os_elf_elf64_half_t e_ehsize;
        os_elf_elf64_half_t e_phentsize;
        os_elf_elf64_half_t e_phnum;
        os_elf_elf64_half_t e_shentsize;
        os_elf_elf64_half_t e_shnum;
        os_elf_elf64_half_t e_shstrndx;
    }os_elf_elf64_ehdr_t,*os_elf_elf64_ehdr_p;

    /* These constants define the permissions on sections in the program
       header, p_flags. */
    #define OS_ELF_PF_R		0x4
    #define OS_ELF_PF_W		0x2
    #define OS_ELF_PF_X		0x1

    typedef struct os_elf_elf32_phdr
    {
        os_elf_elf32_word_t	p_type;
        os_elf_elf32_off_t	p_offset;
        os_elf_elf32_addr_t	p_vaddr;
        os_elf_elf32_addr_t	p_paddr;
        os_elf_elf32_word_t	p_filesz;
        os_elf_elf32_word_t	p_memsz;
        os_elf_elf32_word_t	p_flags;
        os_elf_elf32_word_t	p_align;
    }os_elf_elf32_phdr_t,*os_elf_elf32_phdr_p;

    typedef struct os_elf_elf64_phdr
    {
        os_elf_elf64_word_t p_type;
        os_elf_elf64_word_t p_flags;
        os_elf_elf64_off_t p_offset;		/* Segment file offset */
        os_elf_elf64_addr_t p_vaddr;		/* Segment virtual address */
        os_elf_elf64_addr_t p_paddr;		/* Segment physical address */
        os_elf_elf64_xword_t p_filesz;		/* Segment size in file */
        os_elf_elf64_xword_t p_memsz;		/* Segment size in memory */
        os_elf_elf64_xword_t p_align;		/* Segment alignment, file & memory */
    }os_elf_elf64_phdr_t,*os_elf_elf64_phdr_p;

    /* sh_type */
    #define OS_ELF_SHT_NULL	0
    #define OS_ELF_SHT_PROGBITS	1
    #define OS_ELF_SHT_SYMTAB	2
    #define OS_ELF_SHT_STRTAB	3
    #define OS_ELF_SHT_RELA	4
    #define OS_ELF_SHT_HASH	5
    #define OS_ELF_SHT_DYNAMIC	6
    #define OS_ELF_SHT_NOTE	7
    #define OS_ELF_SHT_NOBITS	8
    #define OS_ELF_SHT_REL		9
    #define OS_ELF_SHT_SHLIB	10
    #define OS_ELF_SHT_DYNSYM	11
    #define OS_ELF_SHT_NUM		12
    #define OS_ELF_SHT_LOPROC	0x70000000
    #define OS_ELF_SHT_HIPROC	0x7fffffff
    #define OS_ELF_SHT_LOUSER	0x80000000
    #define OS_ELF_SHT_HIUSER	0xffffffff

    /* sh_flags */
    #define OS_ELF_SHF_WRITE		0x1
    #define OS_ELF_SHF_ALLOC		0x2
    #define OS_ELF_SHF_EXECINSTR		0x4
    #define OS_ELF_SHF_RELA_LIVEPATCH	0x00100000
    #define OS_ELF_SHF_RO_AFTER_INIT	0x00200000
    #define OS_ELF_SHF_MASKPROC		0xf0000000

    /* special section indexes */
    #define OS_ELF_SHN_UNDEF	0
    #define OS_ELF_SHN_LORESERVE	0xff00
    #define OS_ELF_SHN_LOPROC	0xff00
    #define OS_ELF_SHN_HIPROC	0xff1f
    #define OS_ELF_SHN_LIVEPATCH	0xff20
    #define OS_ELF_SHN_ABS		0xfff1
    #define OS_ELF_SHN_COMMON	0xfff2
    #define OS_ELF_SHN_HIRESERVE	0xffff

    typedef struct os_elf_elf32_shdr
    {
        os_elf_elf32_word_t	sh_name;
        os_elf_elf32_word_t	sh_type;
        os_elf_elf32_word_t	sh_flags;
        os_elf_elf32_addr_t	sh_addr;
        os_elf_elf32_off_t	sh_offset;
        os_elf_elf32_word_t	sh_size;
        os_elf_elf32_word_t	sh_link;
        os_elf_elf32_word_t	sh_info;
        os_elf_elf32_word_t	sh_addralign;
        os_elf_elf32_word_t	sh_entsize;
    }os_elf_elf32_shdr_t,*os_elf_elf32_shdr_p;

    typedef struct os_elf_elf64_shdr
    {
        os_elf_elf64_word_t sh_name;		/* Section name, index in string tbl */
        os_elf_elf64_word_t sh_type;		/* Type of section */
        os_elf_elf64_xword_t sh_flags;		/* Miscellaneous section attributes */
        os_elf_elf64_addr_t sh_addr;		/* Section virtual addr at execution */
        os_elf_elf64_off_t sh_offset;		/* Section file offset */
        os_elf_elf64_xword_t sh_size;		/* Size of section in bytes */
        os_elf_elf64_word_t sh_link;		/* Index of another section */
        os_elf_elf64_word_t sh_info;		/* Additional section information */
        os_elf_elf64_xword_t sh_addralign;	/* Section alignment */
        os_elf_elf64_xword_t sh_entsize;	/* Entry size if section holds table */
    }os_elf_elf64_shdr_t,*os_elf_elf64_shdr_p;

    #define	OS_ELF_EI_MAG0		0		/* e_ident[] indexes */
    #define	OS_ELF_EI_MAG1		1
    #define	OS_ELF_EI_MAG2		2
    #define	OS_ELF_EI_MAG3		3
    #define	OS_ELF_EI_CLASS	4
    #define	OS_ELF_EI_DATA		5
    #define	OS_ELF_EI_VERSION	6
    #define	OS_ELF_EI_OSABI	7
    #define	OS_ELF_EI_PAD		8

    #define	OS_ELF_ELFMAG0		0x7f		/* OS_ELF_EI_MAG */
    #define	OS_ELF_ELFMAG1		'E'
    #define	OS_ELF_ELFMAG2		'L'
    #define	OS_ELF_ELFMAG3		'F'
    #define	OS_ELF_ELFMAG		"\177ELF"
    #define	OS_ELF_SELFMAG		4

    #define	OS_ELF_ELFCLASSNONE	0		/* OS_ELF_EI_CLASS */
    #define	OS_ELF_ELFCLASS32	1
    #define	OS_ELF_ELFCLASS64	2
    #define	OS_ELF_LFCLASSNUM	3

    #define OS_ELF_ELFDATANONE	0		/* e_ident[OS_ELF_EI_DATA] */
    #define OS_ELF_ELFDATA2LSB	1
    #define OS_ELF_ELFDATA2MSB	2

    #define OS_ELF_EV_NONE		0		/* e_version, OS_ELF_EI_VERSION */
    #define OS_ELF_EV_CURRENT	1
    #define OS_ELF_EV_NUM		2

    #define OS_ELF_OSABI_NONE	0
    #define OS_ELF_OSABI_LINUX	3

    #define OS_ELF_OSABI OS_ELF_OSABI_LINUX

    /*
     * Notes used in OS_ELF_ET_CORE. Architectures export some of the arch register sets
     * using the corresponding note types via the PTRACE_GETREGSET and
     * PTRACE_SETREGSET requests.
     * The note name for all these is "LINUX".
     */
    #define OS_ELF_NT_PRSTATUS	1
    #define OS_ELF_NT_PRFPREG	2
    #define OS_ELF_NT_PRPSINFO	3
    #define OS_ELF_NT_TASKSTRUCT	4
    #define OS_ELF_NT_AUXV		6
    /*
     * Note to userspace developers: size of OS_ELF_NT_SIGINFO note may increase
     * in the future to accomodate more fields, don't assume it is fixed!
     */
    #define OS_ELF_NT_SIGINFO      0x53494749
    #define OS_ELF_NT_FILE         0x46494c45
    #define OS_ELF_NT_PRXFPREG     0x46e62b7f      /* copied from gdb5.1/include/elf/common.h */
    #define OS_ELF_NT_PPC_VMX	0x100		/* PowerPC Altivec/VMX registers */
    #define OS_ELF_NT_PPC_SPE	0x101		/* PowerPC SPE/EVR registers */
    #define OS_ELF_NT_PPC_VSX	0x102		/* PowerPC VSX registers */
    #define OS_ELF_NT_PPC_TAR	0x103		/* Target address Register */
    #define OS_ELF_NT_PPC_PPR	0x104		/* Program Priority Register */
    #define OS_ELF_NT_PPC_DSCR	0x105		/* Data Stream Control Register */
    #define OS_ELF_NT_PPC_EBB	0x106		/* Event Based Branch Registers */
    #define OS_ELF_NT_PPC_PMU	0x107		/* Performance Monitor Registers */
    #define OS_ELF_NT_PPC_TM_CGPR	0x108		/* TM checkpointed GPR Registers */
    #define OS_ELF_NT_PPC_TM_CFPR	0x109		/* TM checkpointed FPR Registers */
    #define OS_ELF_NT_PPC_TM_CVMX	0x10a		/* TM checkpointed VMX Registers */
    #define OS_ELF_NT_PPC_TM_CVSX	0x10b		/* TM checkpointed VSX Registers */
    #define OS_ELF_NT_PPC_TM_SPR	0x10c		/* TM Special Purpose Registers */
    #define OS_ELF_NT_PPC_TM_CTAR	0x10d		/* TM checkpointed Target address Register */
    #define OS_ELF_NT_PPC_TM_CPPR	0x10e		/* TM checkpointed Program Priority Register */
    #define OS_ELF_NT_PPC_TM_CDSCR	0x10f		/* TM checkpointed Data Stream Control Register */
    #define OS_ELF_NT_PPC_PKEY	0x110		/* Memory Protection Keys registers */
    #define OS_ELF_NT_386_TLS	0x200		/* i386 TLS slots (struct user_desc) */
    #define OS_ELF_NT_386_IOPERM	0x201		/* x86 io permission bitmap (1=deny) */
    #define OS_ELF_NT_X86_XSTATE	0x202		/* x86 extended state using xsave */
    #define OS_ELF_NT_S390_HIGH_GPRS	0x300	/* s390 upper register halves */
    #define OS_ELF_NT_S390_TIMER	0x301		/* s390 timer register */
    #define OS_ELF_NT_S390_TODCMP	0x302		/* s390 TOD clock comparator register */
    #define OS_ELF_NT_S390_TODPREG	0x303		/* s390 TOD programmable register */
    #define OS_ELF_NT_S390_CTRS	0x304		/* s390 control registers */
    #define OS_ELF_NT_S390_PREFIX	0x305		/* s390 prefix register */
    #define OS_ELF_NT_S390_LAST_BREAK	0x306	/* s390 breaking event address */
    #define OS_ELF_NT_S390_SYSTEM_CALL	0x307	/* s390 system call restart data */
    #define OS_ELF_NT_S390_TDB	0x308		/* s390 transaction diagnostic block */
    #define OS_ELF_NT_S390_VXRS_LOW	0x309	/* s390 vector registers 0-15 upper half */
    #define OS_ELF_NT_S390_VXRS_HIGH	0x30a	/* s390 vector registers 16-31 */
    #define OS_ELF_NT_S390_GS_CB	0x30b		/* s390 guarded storage registers */
    #define OS_ELF_NT_S390_GS_BC	0x30c		/* s390 guarded storage broadcast control block */
    #define OS_ELF_NT_S390_RI_CB	0x30d		/* s390 runtime instrumentation */
    #define OS_ELF_NT_ARM_VFP	0x400		/* ARM VFP/NEON registers */
    #define OS_ELF_NT_ARM_TLS	0x401		/* ARM TLS register */
    #define OS_ELF_NT_ARM_HW_BREAK	0x402		/* ARM hardware breakpoint registers */
    #define OS_ELF_NT_ARM_HW_WATCH	0x403		/* ARM hardware watchpoint registers */
    #define OS_ELF_NT_ARM_SYSTEM_CALL	0x404	/* ARM system call number */
    #define OS_ELF_NT_ARM_SVE	0x405		/* ARM Scalable Vector Extension registers */
    #define OS_ELF_NT_ARM_PAC_MASK		0x406	/* ARM pointer authentication code masks */
    #define OS_ELF_NT_ARM_PACA_KEYS	0x407	/* ARM pointer authentication address keys */
    #define OS_ELF_NT_ARM_PACG_KEYS	0x408	/* ARM pointer authentication generic key */
    #define OS_ELF_NT_ARM_TAGGED_ADDR_CTRL	0x409	/* arm64 tagged address control (prctl()) */
    #define OS_ELF_NT_ARC_V2	0x600		/* ARCv2 accumulator/extra registers */
    #define OS_ELF_NT_VMCOREDD	0x700		/* Vmcore Device Dump Note */
    #define OS_ELF_NT_MIPS_DSP	0x800		/* MIPS DSP ASE registers */
    #define OS_ELF_NT_MIPS_FP_MODE	0x801		/* MIPS floating-point mode */
    #define OS_ELF_NT_MIPS_MSA	0x802		/* MIPS SIMD registers */

    /* Note types with note name "GNU" */
    #define OS_ELF_NT_GNU_PROPERTY_TYPE_0	5

    /* Note header in a OS_ELF_PT_NOTE section */
    typedef struct os_elf_elf32_note
    {
        os_elf_elf32_word_t	n_namesz;	/* Name size */
        os_elf_elf32_word_t	n_descsz;	/* Content size */
        os_elf_elf32_word_t	n_type;		/* Content type */
    }os_elf_elf32_nhdr_t,*os_elf_elf32_nhdr_p;

    /* Note header in a OS_ELF_PT_NOTE section */
    typedef struct os_elf_elf64_note
    {
        os_elf_elf64_word_t n_namesz;	/* Name size */
        os_elf_elf64_word_t n_descsz;	/* Content size */
        os_elf_elf64_word_t n_type;	/* Content type */
    }os_elf_elf64_nhdr_t,*os_elf_elf64_nhdr_p;

    /* .note.gnu.property types for EM_AARCH64: */
    #define OS_ELF_GNU_PROPERTY_AARCH64_FEATURE_1_AND	0xc0000000

    /* Bits for GNU_PROPERTY_AARCH64_FEATURE_1_BTI */
    #define OS_ELF_GNU_PROPERTY_AARCH64_FEATURE_1_BTI	(1U << 0)

    os_err_t os_elf_load(const char *path,os_size_t *entry);

#endif
