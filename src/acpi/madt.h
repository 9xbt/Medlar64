#pragma once

#include <types.h>

typedef struct {
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;

    /* MADT */
    u32 lapic_addr;
    u32 flags;

    char table[];
} acpi_madt;

typedef struct {
    u8 type;
    u8 length;
} madt_entry;

typedef struct {
    madt_entry entry;
    u8 id;
    u8 resv;
    u32 address;
    u32 gsi_base;
} madt_ioapic;

typedef struct {
    madt_entry entry;
    u8 bus_source;
    u8 irq_source;
    u32 gsi;
    u16 flags;
} madt_iso;