#include <mm/pmm.h>
#include <dev/lapic.h>
#include <dev/char/serial.h>

void lapic_calibrate_timer() {
    
}

__attribute__((no_sanitize("undefined")))
u32 lapic_read(u32 reg) {
    return *((u32*)(HIGHER_HALF(LAPIC_REGS) + reg));
}

__attribute__((no_sanitize("undefined")))
void lapic_write(u32 reg, u32 val) {
    *((u32*)(HIGHER_HALF(LAPIC_REGS) + reg)) = val;
}

void lapic_eoi() {
    lapic_write((u8)LAPIC_EOI, 0);
}

void lapic_init() {
    lapic_write(LAPIC_SIV, lapic_read(LAPIC_SIV) | 0x100);
    dprintf("lapic: initialized successfully\n");
}