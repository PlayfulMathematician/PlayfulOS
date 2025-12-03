#include "keyboard.h"
#include "arch/x86_64/interrupts/interrupt.h"
#include "arch/x86_64/interrupts/pic.h"
#include "io/io.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define KEYBOARD_BUFFER_SIZE 256

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile size_t keyboard_buffer_head = 0;
static volatile size_t keyboard_buffer_tail = 0;

static void wait_kbc(void) {
  uint8_t status;
  int timeout = 100000;
  do {
    status = inb(0x64);
    if (!(status & 0x02)) break;
    timeout--;
  } while (timeout > 0);
}

static void keyboard_handler(struct interrupt_context *ctx) {
  (void)ctx;
  
  uint8_t scancode = inb(0x60);
  
  if (scancode == 0xFA || scancode == 0xFE || scancode == 0xAA || scancode == 0xEE) {
    interrupt_send_eoi(1);
    return;
  }
  
  static char keymap[128] = {
      0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
      '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
      'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
      'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
      'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' ',
  };

  if (!(scancode & 0x80) && scancode < 128) {
    char c = keymap[scancode];
    if (c) {
      size_t next_tail = (keyboard_buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
      if (next_tail != keyboard_buffer_head) {
        keyboard_buffer[keyboard_buffer_tail] = c;
        keyboard_buffer_tail = next_tail;
      }
    }
  }

  interrupt_send_eoi(1);
}

void keyboard_enable(void) {
  while (inb(0x64) & 0x01) {
    inb(0x60);
  }
  
  wait_kbc();
  outb(0x64, 0xAE);
  
  wait_kbc();
  outb(0x64, 0x20);
  wait_kbc();
  uint8_t config = inb(0x60);
  
  config |= 0x01;
  
  wait_kbc();
  outb(0x64, 0x60);
  wait_kbc();
  outb(0x60, config);
  
  while (inb(0x64) & 0x01) {
    inb(0x60);
  }
  
  wait_kbc();
  outb(0x60, 0xF4);
  
  interrupt_register(33, keyboard_handler, NULL);
  
  interrupt_enable_irq(1);
}

bool keyboard_has_char(void) {
  return keyboard_buffer_head != keyboard_buffer_tail;
}

char keyboard_getchar(void) {
  if (!keyboard_has_char()) {
    return 0;
  }
  
  char c = keyboard_buffer[keyboard_buffer_head];
  keyboard_buffer_head = (keyboard_buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
  return c;
}
