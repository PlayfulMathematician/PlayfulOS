#include "arch/x86_64/interrupts/idt.h"
#include "arch/x86_64/interrupts/pic.h"
#include "arch/x86_64/interrupts/interrupt.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/tty/tty.h"

void kmain() {
  tty_initialize();
  tty_write("Keyboard demo! Type anything:\n");

  interrupt_disable();
  pic_remap();
  idt_init();
  interrupt_init();
  keyboard_enable();
  interrupt_enable();

  for (;;) {
    while (keyboard_has_char()) {
      char c = keyboard_getchar();
      if (c) {
        tty_putchar(c);
      }
    }
    __asm__("hlt");
  }
}
