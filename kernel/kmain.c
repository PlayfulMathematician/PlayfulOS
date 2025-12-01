#include "arch/x86_64/interrupts/idt.h"
#include "arch/x86_64/interrupts/pic.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/tty/tty.h"

void kmain() {
  tty_initialize();
  tty_write("Keyboard demo! Type anything:\n");

  __asm__ volatile("cli");
  pic_remap();
  idt_init();
  keyboard_enable();
  __asm__ volatile("sti");

  for (;;) {
    __asm__("hlt");
  }
}
