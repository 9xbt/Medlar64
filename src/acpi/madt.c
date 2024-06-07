#include <acpi/acpi.h>
#include <acpi/madt.h>

madt_ioapic *madt_ioapic_list[128];
u32 madt_ioapic_len = 0;

void madt_init() {
    acpi_madt *madt = (acpi_madt*)acpi_find_table("APIC");

    int i = 0;
    for (;;) {
        madt_entry *entry = (madt_entry*)(madt->table + i);

        switch (entry->type) {
            case 1:
                madt_ioapic_list[madt_ioapic_len++] = (madt_ioapic*)entry;
                break;
        }

        i += entry->length;
    }
}