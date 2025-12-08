cmake_minimum_required(VERSION 3.16)
project(PlayfulOS C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(ASM)

add_compile_options(
    -ffreestanding
    -fno-stack-protector
    -fno-pic
    -fno-pie
    -mno-red-zone
    -fno-asynchronous-unwind-tables
)

add_executable(kernel.elf
    kernel/kmain.c
    kernel/drivers/video/vga/vga_text.c
    kernel/drivers/time/pit/pit.c
        kernel/drivers/time/rtc/rtc.c


    kernel/drivers/input/ps2/keyboard.c

    kernel/arch/x86_64/interrupts/idt.c
    kernel/arch/x86_64/interrupts/interrupt.c
    kernel/arch/x86_64/interrupts/pic.c
    kernel/arch/x86_64/interrupts/interrupts.S
    kernel/arch/x86_64/boot/entry.S
)

target_include_directories(kernel.elf PRIVATE ${CMAKE_SOURCE_DIR}/kernel)

target_link_options(kernel.elf PRIVATE
    -nostdlib
    -Wl,-T${CMAKE_SOURCE_DIR}/kernel/linker.ld
    -Wl,-no-pie
    -static
)


