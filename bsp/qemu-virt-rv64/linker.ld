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
        . += 8K;
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