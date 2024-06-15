#include <dev/ioapic.h>
#include <acpi/madt.h>
#include <mm/pmm.h>

__attribute__((no_sanitize("undefined")))
u32 ioapic_read(madt_ioapic* ioapic, u8 reg) {
    u32* ioapic_addr = (u32*)(HIGHER_HALF(ioapic->address));
    ioapic_addr[0] = reg;
    return ioapic_addr[4];
}

__attribute__((no_sanitize("undefined")))
void ioapic_write(madt_ioapic* ioapic, u8 reg, u32 value) {
    u32* ioapic_addr = (u32*)(HIGHER_HALF(ioapic->address));
    ioapic_addr[0] = reg;
    ioapic_addr[4] = value;
}

u64 ioapic_gsi_count(madt_ioapic* ioapic) {
    return (ioapic_read(ioapic, IOAPIC_VER) >> 16) & 0xff;
}

__attribute__((no_sanitize("undefined")))
void ioapic_init() {
    madt_ioapic* ioapic = madt_ioapic_list[0];

    u32 ver = ioapic_read(ioapic, IOAPIC_VER);
    u32 id = ioapic_read(ioapic, IOAPIC_ID) >> 24;

    if (id != ioapic->id) {
        dprintf("ioapic: apic id mismatch\n");
        dprintf("ioapic: expected %u but got %u\n", ioapic->id, id);
        return;
    }

    dprintf("ioapic: initialized ioapic\n");
}