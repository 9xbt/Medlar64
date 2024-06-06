#pragma once

#include <types.h>

typedef struct {
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 rsdt_addr;
} __attribute__((packed)) acpi_rsdp;

void acpi_init();