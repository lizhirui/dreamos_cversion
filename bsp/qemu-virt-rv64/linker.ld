OUTPUT_ARCH(riscv)
ENTRY(_start)

/*IO Mapping 0xFFFF FFC0 0000 0000 ~ 0xFFFF FFFF FFFF FFFF 256GB*/
/*Kernel Mapping 0x0000 0020 0000 0000 ~ 0x0000 003F FFFF FFFF 128GB*/
/*User Mapping 0x0000 0000 0020 0000 ~ 0x0000 001F FFFF FFFF 128GB*/

BASE_ADDRESS = 0x0000002000200000;
/*BASE_ADDRESS = 0x80200000;*/
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
        etext = .;
    }

    . = ALIGN(8);

    .rodata :
    {
        srodata = .;
        *(.rodata .rodata.* .rodata*)
        *(.srodata .srodata.* .srodata*)
        erodata = .;
    }

    . = ALIGN(8);

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

    . = ALIGN(8);

    .osdebug :
    {
        _osdebug_start = .;
        . += 20K;
        _osdebug_end = .;
    }

    . = ALIGN(8);

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
        . = ALIGN(8);
        PROVIDE(__stack_default = .);
        . += STACK_SIZE;
        . = ALIGN(8);
        PROVIDE(__stack_interrupt_cpu0 = .);
        ebss = .;
        . = ALIGN(4K);
        _heap_start = .;
    }

    PROVIDE(end = .);
}