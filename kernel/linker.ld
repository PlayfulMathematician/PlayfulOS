ENTRY(_start)

SECTIONS
{
    . = 1M;

    .multiboot :
    {
        KEEP(*(.multiboot2_header))
    }

    .text :
    {
        *(.text*)
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
        *(COMMON)
    }
}

