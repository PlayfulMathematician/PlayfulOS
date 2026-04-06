#pragma once

#include <stdbool.h>
#include <stdint.h>

#define INT_TYPE_EXCEPTION 0
#define INT_TYPE_IRQ 1
#define INT_TYPE_SOFTWARE 2

struct interrupt_context {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
  uint64_t int_no;
  uint64_t error_code;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} __attribute__((packed));

typedef void (*interrupt_handler_t)(struct interrupt_context *ctx);

struct interrupt_handler {
  interrupt_handler_t handler;
  void *data;
  bool active;
};

void interrupt_init(void);

bool interrupt_register(uint8_t int_no, interrupt_handler_t handler,
                        void *data);

void interrupt_unregister(uint8_t int_no);

static inline void interrupt_enable(void) { __asm__ volatile("sti"); }

static inline void interrupt_disable(void) { __asm__ volatile("cli"); }

static inline bool interrupt_enabled(void) {
  uint64_t rflags;
  __asm__ volatile("pushfq; popq %0" : "=r"(rflags));
  return (rflags & (1 << 9)) != 0;
}

void interrupt_enable_irq(uint8_t irq);
void interrupt_disable_irq(uint8_t irq);

void interrupt_send_eoi(uint8_t irq);

uint8_t interrupt_get_type(uint8_t int_no);
