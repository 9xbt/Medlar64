#include <limine.h>
#include <mm/pmm.h>
#include <dev/char/serial.h>

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

struct limine_memmap_response* pmm_memmap;

u64 pmm_total_pages = 0;

void pmm_init() {
    pmm_memmap = memmap_request.response;
    struct limine_memmap_entry** entries = pmm_memmap->entries;

    dprintf("pmm_init(): entry count: %d\n", pmm_memmap->entry_count);

    for (u64 i = 0; i < pmm_memmap->entry_count; i++) {
        
    }
}