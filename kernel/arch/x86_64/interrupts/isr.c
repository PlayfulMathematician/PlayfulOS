#include "io/io.h"
#include "drivers/tty/tty.h"
#include "pic.h"
#include <stdint.h>

void isr_handler(uint64_t *stack) {
  uint64_t int_no = stack[15];

  if (int_no == 33) {
    uint8_t scancode = inb(0x60);
    
    if (scancode == 0xFA || scancode == 0xFE || scancode == 0xAA || scancode == 0xEE) {
      pic_send_eoi(1);
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
      if (c)
        tty_putchar(c);
    }

    pic_send_eoi(1);
  } else if (int_no >= 32 && int_no <= 47) {
    pic_send_eoi(int_no - 32);
  }
}
