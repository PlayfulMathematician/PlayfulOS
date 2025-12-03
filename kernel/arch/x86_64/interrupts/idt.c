#include "idt.h"

#define IDT_ENTRIES 256

struct idt_entry {
  uint16_t isr_low;
  uint16_t kernel_cs;
  uint8_t ist;
  uint8_t flags;
  uint16_t isr_mid;
  uint32_t isr_high;
  uint32_t reserved;
} __attribute__((packed));

struct idtr {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_ENTRIES];

extern void *isr_stub_table[];

void idt_set_gate(int n, uint64_t handler, uint16_t sel, uint8_t flags) {
  idt_set_gate_full(n, handler, sel, flags, 0);
}

void idt_set_gate_full(int n, uint64_t handler, uint16_t sel, uint8_t flags, uint8_t ist) {
  if (n >= IDT_ENTRIES) return;
  
  idt[n].isr_low = handler & 0xFFFF;
  idt[n].kernel_cs = sel;
  idt[n].ist = ist;
  idt[n].flags = flags;
  idt[n].isr_mid = (handler >> 16) & 0xFFFF;
  idt[n].isr_high = (handler >> 32) & 0xFFFFFFFF;
  idt[n].reserved = 0;
}

void idt_init(void) {
  struct idtr idtr;
  idtr.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
  idtr.base = (uint64_t)idt;

  for (int i = 0; i < IDT_ENTRIES; i++) {
    idt_set_gate(i, (uint64_t)isr_stub_table[i], 0x08, 0x8E);
  }

  __asm__ volatile("lidt %0" : : "m"(idtr));
}
