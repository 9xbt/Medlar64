#pragma once

#include <types.h>

#define LAPIC_REGS 0xfee00000
#define LAPIC_EOI 0xb0
#define LAPIC_SIV 0xf0

void lapic_init();