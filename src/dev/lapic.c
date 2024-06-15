#include <mm/pmm.h>
#include <dev/pit.h>
#include <dev/lapic.h>
#include <dev/char/serial.h>

u64 lapic_ticks = 0;

void lapic_stop_timer() {
    lapic_write(LAPIC_TIMER_INITCNT, 0);
    lapic_write(LAPIC_TIMER_LVT, LAPIC_TIMER_DISABLE);
}

void lapic_oneshot(u8 vector, u64 ms) {
    lapic_stop_timer();

    lapic_write(LAPIC_TIMER_DIV, 0);
    lapic_write(LAPIC_TIMER_LVT, vector);
    lapic_write(LAPIC_TIMER_INITCNT, lapic_ticks * ms);
}

void lapic_calibrate_timer() {
    lapic_stop_timer();

    lapic_write(LAPIC_TIMER_DIV, 0);
    lapic_write(LAPIC_TIMER_LVT, (1 << 16) | 0xff);
    lapic_write(LAPIC_TIMER_INITCNT, 0xFFFFFFFF);

    pit_sleep(1);

    lapic_write(LAPIC_TIMER_LVT, LAPIC_TIMER_DISABLE);

    u32 ticks = 0xFFFFFFFF - lapic_read(LAPIC_TIMER_CURCNT);
    lapic_ticks = ticks;

    lapic_stop_timer();

    dprintf("lapic: calibrated timer\n");
}

__attribute__((no_sanitize("undefined")))
u32 lapic_read(u32 reg) {
    return *((u32*)(HIGHER_HALF(LAPIC_REGS) + reg));
}

__attribute__((no_sanitize("undefined")))
void lapic_write(u32 reg, u32 value) {
    *((u32*)(HIGHER_HALF(LAPIC_REGS) + reg)) = value;
}

void lapic_eoi() {
    lapic_write((u8)LAPIC_EOI, 0);
}

void lapic_init() {
    lapic_write(LAPIC_SIV, lapic_read(LAPIC_SIV) | 0x100);
    dprintf("lapic: initialized successfully\n");
}