#include <sys/idt.h>

__attribute__((aligned(0x10)))
static idt_entry idt_entries[256];
static idtr idt_descriptor;

void idt_init() {
    for (u8 i = 0; i < 256; i++) {
        idt_set_entry(i, NULL, i == 0x80 ? 0xEE : 0x8E);
    }

    idt_descriptor = (idtr) {
        .size = sizeof(idt_entry) * 256 - 1,
        .offset = (u64)idt_entries
    };

    asm volatile ("lidt %0" :: "m"(idt_descriptor) : "memory");
    asm volatile ("sti");
}

void idt_set_entry(u8 vec, void* isr, u8 flags) {
    idt_entries[vec].offset_low  = (u64)isr & 0xFFFF;
    idt_entries[vec].cs   = 0x28;
    idt_entries[vec].ist  = 0;
    idt_entries[vec].flags = flags;
    idt_entries[vec].offset_mid  = ((u64)isr >> 16) & 0xFFFF;
    idt_entries[vec].offset_high = ((u64)isr >> 32) & 0xFFFFFFFF;
    idt_entries[vec].reserved = 0;
}