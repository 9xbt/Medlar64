#pragma once

#include <types.h>

#define LAPIC_REGS          0xfee00000
#define LAPIC_EOI           0xb0
#define LAPIC_SIV           0xf0

#define LAPIC_TIMER_DIV     0x3E0
#define LAPIC_TIMER_LVT     0x320
#define LAPIC_TIMER_INITCNT 0x380
#define LAPIC_TIMER_DISABLE 0x10000
#define LAPIC_TIMER_CURCNT  0x390

void lapic_init();

void lapic_stop_timer();
void lapic_oneshot(u8 vector, u64 ms);
void lapic_calibrate_timer();

u32 lapic_read(u32 reg);
void lapic_write(u32 reg, u32 val);

void lapic_eoi();