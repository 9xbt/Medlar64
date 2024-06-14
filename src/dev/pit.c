#include <dev/lapic.h>
#include <dev/pit.h>
#include <sys/idt.h>
#include <sys/io.h>

u64 pit_ticks = 0;

void pit_handler(regs* r) {
    (void)r;
    pit_ticks++;
    lapic_eoi();
}

void pit_init() {
    u16 divisor = (u16)(1193180 / PIT_FREQ);

    outb(PIT_CMD, 0x36);
    outb(PIT_DAT, (u8)divisor);
    outb(PIT_DAT, (u8)(divisor >> 8));

    irq_register(0, pit_handler);
}

void pit_sleep(u64 ms) {
    u64 start_ticks = pit_ticks;
    u64 end_ticks = start_ticks + ms;

    while (pit_ticks < end_ticks) {
        asm volatile ("hlt");
    }
}