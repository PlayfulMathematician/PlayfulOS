#include "tty.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t *)0xB8000)

static uint16_t *tty_buffer = VGA_MEMORY;
static size_t tty_row = 0;
static size_t tty_column = 0;
static uint8_t tty_color = 0x07;

static inline uint8_t vga_color(uint8_t fg, uint8_t bg) {
  return fg | (bg << 4);
}

static inline uint16_t vga_entry(unsigned char ch, uint8_t color) {
  return (uint16_t)ch | ((uint16_t)color << 8);
}

void tty_clear(void) {
  tty_row = 0;
  tty_column = 0;
  tty_color = vga_color(7, 0);

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      tty_buffer[y * VGA_WIDTH + x] = vga_entry(' ', tty_color);
    }
  }
}

void tty_initialize(void) { tty_clear(); }

static void tty_scroll(void) {
  for (size_t y = 1; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      tty_buffer[(y - 1) * VGA_WIDTH + x] = tty_buffer[y * VGA_WIDTH + x];
    }
  }

  for (size_t x = 0; x < VGA_WIDTH; x++) {
    tty_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', tty_color);
  }

  if (tty_row > 0)
    tty_row--;
}

void tty_putchar(char c) {
  if (c == '\n') {
    tty_column = 0;
    tty_row++;
    if (tty_row >= VGA_HEIGHT)
      tty_scroll();
    return;
  }

  if (c == '\b') {
    if (tty_column > 0) {
      tty_column--;
    } else if (tty_row > 0) {
      tty_row--;
      tty_column = VGA_WIDTH - 1;
    }
    tty_buffer[tty_row * VGA_WIDTH + tty_column] = vga_entry(' ', tty_color);
    return;
  }

  tty_buffer[tty_row * VGA_WIDTH + tty_column] = vga_entry(c, tty_color);

  tty_column++;
  if (tty_column >= VGA_WIDTH) {
    tty_column = 0;
    tty_row++;
    if (tty_row >= VGA_HEIGHT)
      tty_scroll();
  }
}

void tty_write_len(const char *str, size_t size) {
  for (size_t i = 0; i < size; i++)
    tty_putchar(str[i]);
}

void tty_write(const char *str) {
  while (*str)
    tty_putchar(*str++);
}
