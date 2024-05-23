#include <sys/io.h>
#include <dev/pic.h>

void pic_remap() {
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DAT, 0x20);
    outb(PIC2_DAT, 0x28);
    outb(PIC1_DAT, 0x04);
    outb(PIC2_DAT, 0x02);
    outb(PIC1_DAT, ICW4_8086);
    outb(PIC2_DAT, ICW4_8086);

    outb(PIC1_DAT, 0);
    outb(PIC2_DAT, 0);
}

void pic_eoi(u8 irq) {
    if (irq >= 8) {
        outb(PIC2, PIC_EOI);
    }
    outb(PIC1, PIC_EOI);
}