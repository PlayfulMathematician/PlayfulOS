#include "keyboard.h"
#include "io/io.h"
#include <stdint.h>

static void wait_kbc(void) {
  uint8_t status;
  int timeout = 100000;
  do {
    status = inb(0x64);
    if (!(status & 0x02)) break;
    timeout--;
  } while (timeout > 0);
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
}
