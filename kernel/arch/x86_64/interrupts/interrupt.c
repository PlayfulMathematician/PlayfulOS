#include "interrupt.h"
#include "drivers/io/io.h"
#include "pic.h"
#include <stdbool.h>
#include <stddef.h>

#define MAX_HANDLERS 256
#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)

static struct interrupt_handler handlers[MAX_HANDLERS];

static void default_exception_handler(struct interrupt_context *ctx) {
  (void)ctx;
  for (;;) {
    __asm__ volatile("hlt");
  }
}

static void default_irq_handler(struct interrupt_context *ctx) {
  uint8_t int_no = ctx->int_no;

  if (int_no >= 32 && int_no <= 47) {
    interrupt_send_eoi(int_no - 32);
  }
}

uint8_t interrupt_get_type(uint8_t int_no) {
  if (int_no < 32) {
    return INT_TYPE_EXCEPTION;
  } else if (int_no >= 32 && int_no <= 47) {
    return INT_TYPE_IRQ;
  } else {
    return INT_TYPE_SOFTWARE;
  }
}

void interrupt_init(void) {
  for (int i = 0; i < MAX_HANDLERS; i++) {
    handlers[i].handler = NULL;
    handlers[i].data = NULL;
    handlers[i].active = false;
  }

  for (int i = 0; i < 32; i++) {
    interrupt_register(i, default_exception_handler, NULL);
  }

  for (int i = 32; i < 48; i++) {
    interrupt_register(i, default_irq_handler, NULL);
  }
}

bool interrupt_register(uint8_t int_no, interrupt_handler_t handler,
                        void *data) {
  if (int_no >= MAX_HANDLERS || handler == NULL) {
    return false;
  }

  handlers[int_no].handler = handler;
  handlers[int_no].data = data;
  handlers[int_no].active = true;

  return true;
}

void interrupt_unregister(uint8_t int_no) {
  if (int_no >= MAX_HANDLERS) {
    return;
  }

  handlers[int_no].handler = NULL;
  handlers[int_no].data = NULL;
  handlers[int_no].active = false;
}

void interrupt_enable_irq(uint8_t irq) {
  if (irq > 15)
    return;

  uint16_t port;
  uint8_t value;

  if (irq < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq -= 8;
  }

  value = inb(port) & ~(1 << irq);
  outb(port, value);
}

void interrupt_disable_irq(uint8_t irq) {
  if (irq > 15)
    return;

  uint16_t port;
  uint8_t value;

  if (irq < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq -= 8;
  }

  value = inb(port) | (1 << irq);
  outb(port, value);
}

void interrupt_send_eoi(uint8_t irq) { pic_send_eoi(irq); }

void isr_handler(struct interrupt_context *ctx) {
  uint8_t int_no = ctx->int_no;

  if (int_no < MAX_HANDLERS && handlers[int_no].active &&
      handlers[int_no].handler != NULL) {
    handlers[int_no].handler(ctx);
  } else {
    if (int_no < 32) {
      default_exception_handler(ctx);
    } else {
      default_irq_handler(ctx);
    }
  }
}
