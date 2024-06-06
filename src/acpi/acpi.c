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

void *acpi_find_table(const char *name) {

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