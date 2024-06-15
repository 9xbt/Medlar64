#pragma once

#define IOAPIC_ID  0x0
#define IOAPIC_VER 0x1
#define IOAPIC_ARB 0x2

#define IOAPIC_REGSEL 0x0
#define IOAPIC_IOWIN  0x10

void ioapic_init();