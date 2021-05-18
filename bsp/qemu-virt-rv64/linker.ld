OUTPUT_ARCH(riscv)
ENTRY(_start)

BASE_ADDRESS = 0x80200000;
STACK_SIZE = 16K;

SECTIONS
{
    . = BASE_ADDRESS;
    start = .;

    .text :
    {
        stext = .;
        *(.text.entry)
        *(.text .text.*)
        . = ALIGN(4K);
        etext = .;
    }

    .rodata :
    {
        srodata = .;
        *(.rodata .rodata.* .rodata*)
        *(.srodata .srodata.* .srodata*)
        . = ALIGN(4K);
        erodata = .;
    }

    .eh_frame_hdr : 
    { 
         *(.eh_frame_hdr) 
         *(.eh_frame_entry)
    }

    .eh_frame : ONLY_IF_RO { KEEP (*(.eh_frame)) }

    . = ALIGN(8);

    .data :
    {
        sdata = .;
        *(.data .data.*)
        . = ALIGN(8);
        PROVIDE( __global_pointer$ = . + 0x800 );
        *(.sdata .sdata.*)
        edata = .;
    }

    .bss :
    {
        sbss = .;
        *(.sbss .sbss.*)
        *(.dynsbss)
        *(.scommon)
        *(.bss .bss.*)
        *(.dynbss)
        *(COMMON)
        . = ALIGN(4K);
        PROVIDE(__stack_start = .);
        . += STACK_SIZE;
        PROVIDE(__stack_default = .);
        . += STACK_SIZE;
        PROVIDE(__stack_interrupt_cpu0 = .);
        ebss = .;
        . = ALIGN(4K);
        _heap_start = .;
    }

    PROVIDE(end = .);

    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
     * Symbols in the DWARF debugging sections are relative to the beginning
     * of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}