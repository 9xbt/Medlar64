#include <sys/idt.h>
#include <dev/char/serial.h>

__attribute__((aligned(0x10)))
static idt_entry idt_entries[256];
static idtr idt_descriptor;
extern void* idt_int_table[];

void* irq_handlers[256] = {};

static const char* isr_errors[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void idt_init() {
    for (u16 i = 0; i < 256; i++) {
        idt_set_entry(i, idt_int_table[i], i == 0x80 ? 0xEE : 0x8E);
    }
    dprintf("idt: successfully populated 256 entries\n");

    idt_descriptor = (idtr) {
        .size = sizeof(idt_entry) * 256 - 1,
        .offset = (u64)idt_entries
    };
    dprintf("idt: idt descriptor located at %lx\n", (u64)&idt_descriptor);

    asm volatile ("lidt %0" :: "m"(idt_descriptor));
    asm volatile ("sti");
}

void idt_set_entry(u8 vector, void* isr, u8 flags) {
    idt_entries[vector].offset_low  = (u64)isr & 0xFFFF;
    idt_entries[vector].cs   = 0x28;
    idt_entries[vector].ist  = 0;
    idt_entries[vector].flags = flags;
    idt_entries[vector].offset_mid  = ((u64)isr >> 16) & 0xFFFF;
    idt_entries[vector].offset_high = ((u64)isr >> 32) & 0xFFFFFFFF;
    idt_entries[vector].reserved = 0;
}

void irq_register(u8 vector, void* handler) {
    irq_handlers[vector] = handler;
}

void irq_unregister(u8 vector) {
    irq_handlers[vector] = NULL;
}

void isr_handler(regs* r) {
    if (r->int_no == 0xff)
        return; // spurious interrupt

    if (r->int_no == 0x80) {
        printf("Syscall!\n"); // TODO: handle syscalls properly
        return;
    }

    asm volatile ("cli");
    dprintf("isr_handler(): %s! RIP: 0x%llx CS: 0x%x SS: 0x%x\n", isr_errors[r->int_no], r->rip, r->cs, r->ss);
    for (;;) asm volatile ("hlt");
}

void irq_handler(regs* r) {
    void(*handler)(regs*);
    handler = irq_handlers[r->int_no - 32];

    if (handler != NULL)
        handler(r);
}