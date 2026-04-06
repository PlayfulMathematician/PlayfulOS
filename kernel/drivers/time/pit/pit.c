#include "drivers/time/pit/pit.h"
#include "arch/x86_64/interrupts/interrupt.h"
#include "drivers/io/io.h"

static volatile uint64_t pit_tick_count = 0;

uint64_t pit_ticks(void) { return pit_tick_count; }

void pit_handle_irq(struct interrupt_context *ctx) {
  (void)ctx;

  pit_tick_count++;

  interrupt_send_eoi(0);
}

void pit_init(uint32_t frequency) {
  uint32_t divisor = PIT_INPUT_FREQUENCY / frequency;

  outb(0x43, 0x34);

  outb(0x40, divisor & 0xFF);
  outb(0x40, (divisor >> 8) & 0xFF);

  interrupt_register(32, pit_handle_irq, (void *)0);

  interrupt_enable_irq(0);
}

void pit_sleep_ms(uint64_t ms) {
  uint64_t target = pit_ticks() + ms;
  while (pit_ticks() < target) {
    asm volatile("hlt");
  }
}
