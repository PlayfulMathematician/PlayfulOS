#include "arch/x86_64/interrupts/idt.h"
#include "arch/x86_64/interrupts/interrupt.h"
#include "drivers/input/ps2/keyboard.h"

#include "arch/x86_64/interrupts/pic.h"
#include "drivers/time/pit/pit.h"
#include "drivers/time/rtc/rtc.h"
#include "drivers/video/vga/vga_text.h"

int strcmp(const char *a, const char *b) {
  while (*a && (*a == *b)) {
    a++;
    b++;
  }
  return (unsigned char)(*a) - (unsigned char)(*b);
}

void handle_command(const char *cmd) {
  if (strcmp(cmd, "help") == 0) {
    tty_write("Commands:\n");
    tty_write("  help  - show help\n");
    tty_write("  time  - show current RTC time\n");
    tty_write("  wait X - wait X seconds\n");
  }

  else if (strcmp(cmd, "time") == 0) {
    rtc_time_t t;
    rtc_read(&t);

    char buf[32];
    buf[0] = '0' + (t.hour / 10);
    buf[1] = '0' + (t.hour % 10);
    buf[2] = ':';
    buf[3] = '0' + (t.minute / 10);
    buf[4] = '0' + (t.minute % 10);
    buf[5] = ':';
    buf[6] = '0' + (t.second / 10);
    buf[7] = '0' + (t.second % 10);
    buf[8] = 0;

    tty_write("Current Time: ");
    tty_write(buf);
    tty_write("\n");
  }

  else if (cmd[0] == 'w' && cmd[1] == 'a' && cmd[2] == 'i' && cmd[3] == 't') {
    int seconds = cmd[5] - '0';
    if (cmd[6] != '\0') {
      seconds *= 10;
      int seconds = cmd[6] - '0';
    }
    tty_write("Waiting...\n");
    pit_sleep_ms(seconds * 1000);
    tty_write("Done.\n");
  }

  else {
    tty_write("Unknown command: ");
    tty_write(cmd);
    tty_write("\n");
  }
}

void kmain() {

  tty_initialize();
  vga_disable_cursor();
  interrupt_disable();
  pic_remap();
  idt_init();
  interrupt_init();
  keyboard_enable();
  pit_init(1000);
  interrupt_enable();

  tty_write("PlayfulOS Terminal\n");
  tty_write("> ");

  char buffer[128];
  int len = 0;

  while (1) {
    if (keyboard_has_char()) {
      char c = keyboard_getchar();

      if (c == '\n') {
        tty_write("\n");
        buffer[len] = 0;
        handle_command(buffer);
        len = 0;
        tty_write("> ");
      } else if (c == '\b') {
        if (len > 0) {
          len--;
          tty_write("\b \b");
        }
      } else {
        if (len < 127) {
          buffer[len++] = c;
          tty_putchar(c);
        }
      }
    }
  }
}
