.set MB2_MAGIC, 0xe85250d6
.set MB2_ARCH,  0
.set MB2_LEN,   multiboot_end - multiboot_start
.set MB2_CHKSUM, -(MB2_MAGIC + MB2_ARCH + MB2_LEN)

.section .multiboot2_header,"a",@progbits

.align 8
multiboot_start:
    .long MB2_MAGIC
    .long MB2_ARCH
    .long MB2_LEN
    .long MB2_CHKSUM

    .short 0
    .short 0
    .long 8
multiboot_end:



.section .text
.code32
.global _start
.extern kmain

_start:
    cli

    lgdt gdt_ptr

    mov %cr4, %eax
    or  $0x20, %eax
    mov %eax, %cr4

    mov $pml4, %eax
    mov %eax, %cr3

    mov $0xC0000080, %ecx
    rdmsr
    or $(1 << 8), %eax
    wrmsr

    mov %cr0, %eax
    or $(1 << 31), %eax
    mov %eax, %cr0

    ljmp $0x08, $long_mode_start


.section .text
.code64
long_mode_start:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    mov %cr0, %rax
    and $~((1 << 2) | (1 << 3)), %rax
    or $(1 << 1), %rax
    mov %rax, %cr0

    mov %cr4, %rax
    or $((1 << 9) | (1 << 10)), %rax
    mov %rax, %cr4

    fninit

    call kmain

halt:
    hlt
    jmp halt


.section .data
.align 8
gdt64:
    .quad 0
    .quad 0x00af9a000000ffff
    .quad 0x00af92000000ffff

gdt_ptr:
    .word gdt_end - gdt64 - 1
    .quad gdt64
gdt_end:



.align 4096
pml4:
    .quad pdpt + 0x03

.align 4096
pdpt:
    .quad pd + 0x03
    .fill 511,8,0

.align 4096
pd:
    .quad 0x0000000000000083
    .fill 511,8,0

