#include <dev/char/serial.h>
#include <acpi/acpi.h>
#include <lib/libc.h>
#include <limine.h>
#include <kernel.h>

struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

void *acpi_find_table(const char *name) {
    
}

void acpi_init() {
    void *acpi_addr = rsdp_request.response->address;
    acpi_rsdp *rsdp = (acpi_rsdp*)acpi_addr;

    if (memcmp(rsdp->signature, "RSD PTR ", 8) || !rsdp->rsdt_addr) {
        dprintf("acpi: \033[91mfatal error:\033[0m couldn't find acpi\n");
        hcf();
    }

    dprintf("acpi: root table: %lx\n", (u64)acpi_addr);
}