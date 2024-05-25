#include <limine.h>
#include <mm/pmm.h>
#include <dev/char/serial.h>

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

struct limine_memmap_response* pmm_memmap;

u8* pmm_bitmap;
u64 pmm_used_pages = 0;
u64 pmm_page_count = 0;

void pmm_init() {
    pmm_memmap = memmap_request.response;
    struct limine_memmap_entry** entries = pmm_memmap->entries;

    dprintf("pmm_init(): entry count: %d\n", pmm_memmap->entry_count);

    u64 higher_address = 0;
    u64 top_address = 0;

    for (u64 i = 0; i < pmm_memmap->entry_count; i++) {
        if (entries[i]->type != LIMINE_MEMMAP_USABLE) continue;
        top_address = entries[i]->base + entries[i]->length;
        if (top_address > higher_address)
            higher_address = top_address;

        dprintf("pmm_init(): entry length: %" PRId64 "\n", entries[i]->length);
    }

    pmm_page_count = higher_address / PAGE_SIZE;
    u64 bitmap_size = ALIGN_UP(pmm_page_count / 8, PAGE_SIZE);

    dprintf("pmm_init(): total pages: %" PRId64 "\n", pmm_page_count);
    dprintf("pmm_init(): bitmap size: %" PRId64 "\n", bitmap_size);

    for (u64 i = 0; i < pmm_memmap->entry_count; i++) {
        if (entries[i]->type != LIMINE_MEMMAP_USABLE || entries[i]->length < bitmap_size) continue;
    }
}