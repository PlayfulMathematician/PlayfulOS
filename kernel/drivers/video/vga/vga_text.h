#pragma once
#include <stddef.h>
#include <stdint.h>

void tty_initialize(void);
void tty_clear(void);
void tty_write(const char *str);
void tty_write_len(const char *str, size_t len);
void tty_putchar(char c);
void vga_disable_cursor(void);
