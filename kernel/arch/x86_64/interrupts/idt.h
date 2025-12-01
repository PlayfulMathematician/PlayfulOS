#pragma once
#include <stdint.h>

void idt_init(void);
void idt_set_gate(int n, uint64_t handler, uint16_t sel, uint8_t flags);
