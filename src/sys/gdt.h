#pragma once

#include <types.h>

typedef struct {
    u64 entries[9];
} __attribute__((packed)) gdt_table;

typedef struct {
    u16 size;
    u64 offset;
} __attribute__((packed)) gdtr;

void gdt_init();