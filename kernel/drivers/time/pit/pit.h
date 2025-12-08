#pragma once
#include "arch/x86_64/interrupts/interrupt.h"
#include <stdint.h>
#define PIT_INPUT_FREQUENCY 1193182

void pit_init(uint32_t frequency);

uint64_t pit_ticks(void);

void pit_handle_irq(struct interrupt_context *ctx);
void pit_sleep_ms(uint64_t ms);

