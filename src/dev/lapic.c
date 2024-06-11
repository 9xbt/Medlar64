#include <mm/pmm.h>
#include <dev/lapic.h>
#include <dev/char/serial.h>

__attribute__((no_sanitize("undefined")))
u32 lapic_read(u32 reg) {
    return *((u32*)(HIGHER_HALF(LAPIC_REGS) + reg));
}

__attribute__((no_sanitize("undefined")))
void lapic_write(u32 reg, u32 val) {
    *((u32*)(HIGHER_HALF(LAPIC_REGS) + reg)) = val;
}

void lapic_eoi() {
    lapic_write((u8)0xb0, 0);
}

void lapic_init() {
    lapic_write(0xf0, lapic_read(0xf0) | 0x100);
    dprintf("lapic: initialized successfully\n");
}