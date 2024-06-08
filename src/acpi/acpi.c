#include <dev/char/serial.h>
#include <acpi/acpi.h>
#include <lib/libc.h>
#include <mm/pmm.h>
#include <limine.h>
#include <kernel.h>

struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

bool acpi_use_xsdt = false;

void *acpi_root_sdt;

__attribute__((no_sanitize("alignment")))
void *acpi_find_table(const char *signature) {
    if (!acpi_use_xsdt) {
        acpi_rsdt *rsdt = (acpi_rsdt*)acpi_root_sdt;
        u32 entries = (rsdt->sdt.length - sizeof(rsdt->sdt)) / 4;
        
        for (u32 i = 0; i < entries; i++) {
            acpi_sdt *sdt = (acpi_sdt*)HIGHER_HALF(*((u32*)rsdt->table + i));
            if (!memcmp(sdt->signature, signature, 4)) return (void*)sdt;
        }
        return NULL;
    }
    
    /* use xsdt */
    acpi_xsdt *rsdt = (acpi_xsdt*)acpi_root_sdt;
    u32 entries = (rsdt->sdt.length - sizeof(rsdt->sdt)) / 8;
        
    for (u32 i = 0; i < entries; i++) {
        acpi_sdt *sdt = (acpi_sdt*)HIGHER_HALF(*((u64*)rsdt->table + i));
        if (!memcmp(sdt->signature, signature, 4)) return (void*)sdt;
    }
    return NULL;
}

void acpi_init() {
    void *acpi_addr = rsdp_request.response->address;
    acpi_rsdp *rsdp = (acpi_rsdp*)acpi_addr;

    if (memcmp(rsdp->signature, "RSD PTR ", 8) || !rsdp->rsdt_addr) {
        dprintf("acpi: \033[91mfatal error:\033[0m couldn't find acpi\n");
        hcf();
    }

    if (rsdp->revision != 0) {
        /* use xsdt */
        acpi_use_xsdt = true;
        acpi_xsdp *xsdp = (acpi_xsdp*)acpi_addr;
        acpi_root_sdt = (acpi_xsdt*)HIGHER_HALF(xsdp->xsdt_addr);
    } else {
        acpi_root_sdt = (acpi_xsdt*)HIGHER_HALF(rsdp->rsdt_addr);
    }

    dprintf("acpi: root table: %lx\n", (u64)acpi_addr);
    dprintf("acpi: rsdt: %lx\n", (u64)acpi_root_sdt);
}