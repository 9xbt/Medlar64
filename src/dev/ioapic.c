#include <dev/char/serial.h>
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

madt_ioapic* ioapic_get_gsi(u32 gsi) {
    for (u64 i = 0; i < madt_ioapics; i++)
        if (madt_ioapic_list[i]->gsi_base <= gsi && madt_ioapic_list[i]->gsi_base + ioapic_gsi_count(madt_ioapic_list[i]) > gsi)
            return madt_ioapic_list[i];
    return NULL;
}

void ioapic_redirect_gsi(u32 lapic_id, u8 vector, u32 gsi, u16 flags, bool mask) {
    madt_ioapic* ioapic = ioapic_get_gsi(gsi);

    u64 redirect = vector;

    if (flags & (1 << 1)) redirect |= (1 << 13); /* delivery mode */
    if (flags & (1 << 3)) redirect |= (1 << 15); /* destination mode */

    /* interrupt mask */
    if (mask) redirect |= (1 << 16);
    else redirect &= ~(1 << 16);

    redirect |= (uint64_t)lapic_id << 56; /* set destination field */

    u32 redirect_table = (gsi - ioapic->gsi_base) * 2 + 16; /* calculate the offset */
    ioapic_write(ioapic, redirect_table, (u32)redirect); /* low 32 bits */
    ioapic_write(ioapic, redirect_table + 1, (u32)(redirect >> 32)); /* high 32 bits */
}

void ioapic_redirect_irq(u32 lapic_id, u8 vector, u8 irq, bool mask) {
    u8 index = 0;
    madt_iso* iso = NULL;

    while (index < madt_isos) {
        if (madt_iso_list[index]->irq_source == irq) {
            ioapic_redirect_gsi(lapic_id, vector, iso->gsi, madt_iso_list[index]->flags, mask);
            return;
        }
        index++;
    }

    ioapic_redirect_gsi(lapic_id, vector, irq, 0, mask);
}

__attribute__((no_sanitize("undefined")))
void ioapic_init() {
    madt_ioapic* ioapic = madt_ioapic_list[0];

    u32 id = ioapic_read(ioapic, IOAPIC_ID) >> 24;
    u32 ver = ioapic_read(ioapic, IOAPIC_VER);
    u32 count = ioapic_gsi_count(ioapic);

    if (id != ioapic->id) {
        dprintf("ioapic: apic id mismatch\n");
        dprintf("ioapic: expected %u but got %u\n", ioapic->id, id);
        return;
    }

    for (u32 i = 0; i <= count; i++) {
        ioapic_write(ioapic, IOAPIC_REDTBL + (2 * i), 0x10000 | (i + 32)); /* low 32 bits - mask the interrupt */
        ioapic_write(ioapic, IOAPIC_REDTBL + (2 * i) + 1, 0); /* high 32 bits - redirect to cpu 0 */
    }

    dprintf("ioapic: initialized ioapic with %d interrupts\n", count + 1);
}