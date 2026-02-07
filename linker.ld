ENTRY(main)
SECTIONS
{
    . = 0x20000;
    .text : ALIGN(0x1) {
        *(.text.entry)
        *(.text*)
    }
    .data : ALIGN(0x1) { *(.data*) }
    .bss  : ALIGN(0x1) { *(.bss*) }
    /DISCARD/ : { *(.pdata) *(.xdata) *(.comment) }
}