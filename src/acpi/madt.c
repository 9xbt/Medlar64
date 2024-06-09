#include <acpi/acpi.h>
#include <acpi/madt.h>

madt_ioapic *madt_ioapic_list[128];
madt_iso *madt_iso_list[128];
u32 madt_ioapic_len = 0;
u32 madt_iso_len = 0;

madt_lapic_addr *lapic_addr;

__attribute__((no_sanitize("alignment")))
void madt_init() {
    acpi_madt *madt = (acpi_madt*)acpi_find_table("APIC");

    int i = 0;
    while (i <= madt->length - sizeof(madt)) {
        madt_entry *entry = (madt_entry*)(madt->table + i);

        switch (entry->type) {
            case 1:
                madt_ioapic_list[madt_ioapic_len++] = (madt_ioapic*)entry;
                break;
            case 2:
                madt_iso_list[madt_iso_len++] = (madt_iso*)entry;
                break;
            case 5:
                lapic_addr = (madt_lapic_addr*)entry;
                break;
        }

        i += entry->length;
    }
}