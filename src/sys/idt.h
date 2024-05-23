#pragma once

#include <types.h>

typedef struct {
    u16 offset_low;
    u16 cs;
    u8  ist;
    u8  flags;
    u16 offset_mid;
    u32 offset_high;
    u32 reserved;
} __attribute__((packed)) idt_entry;

typedef struct {
    u16 size;
    u64 offset;
} __attribute__((packed)) idtr;

void idt_init();
void idt_set_entry(u8 vec, void* isr, u8 flags);