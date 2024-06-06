#pragma once

#include <types.h>

typedef struct {
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 rsdt_addr;
} __attribute__((packed)) acpi_rsdp;

typedef struct {
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 rsdt_addr;

    u32 length;
    u64 xsdt_addr;
    u8 extended_checksum;
    u8 reserved[3];
} __attribute__((packed)) acpi_xsdp;

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
} __attribute__((packed)) acpi_sdt;

typedef struct {
    acpi_sdt sdt;
    char table[];
} acpi_rsdt;

typedef struct {
    acpi_sdt sdt;
    char table[];
} acpi_xsdt;

void acpi_init();