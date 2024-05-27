#include <limine.h>
#include <mm/pmm.h>
#include <mm/bitmap.h>
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
    /* TODO: use largest all memmap entries if possible */
    pmm_memmap = memmap_request.response;
    struct limine_memmap_entry** entries = pmm_memmap->entries;

    dprintf("pmm_init(): entry count: %ld\n", pmm_memmap->entry_count);

    u64 higher_address = 0;
    u64 top_address = 0;

    u64 i;
    for (i = 0; i < pmm_memmap->entry_count; i++) {
        if (entries[i]->type != LIMINE_MEMMAP_USABLE) continue; /* ignore unusable memmaps */
        top_address = entries[i]->base + entries[i]->length;
        if (top_address > higher_address)
            higher_address = top_address;
    }

    pmm_page_count = higher_address / PAGE_SIZE;
    u64 bitmap_size = ALIGN_UP(pmm_page_count / 8, PAGE_SIZE);

    dprintf("pmm_init(): total pages: %ld\n", pmm_page_count);
    dprintf("pmm_init(): bitmap size: %ld\n", bitmap_size);

    for (i = 0; i < pmm_memmap->entry_count; i++) {
        if (entries[i]->type != LIMINE_MEMMAP_USABLE || entries[i]->length < bitmap_size) continue;
        /* we found a usuable memmap! use it! */
        pmm_bitmap = (u8*)HIGHER_HALF(entries[i]->base); /* use the higher half of memory */
        /* since we're storing the bitmap at the start of the memmap, subtract */
        /* the size of the bitmap to the memmap entry */
        entries[i]->base += bitmap_size;
        entries[i]->length -= bitmap_size;
        break;
    }

    for (i = 0; i < pmm_memmap->entry_count; i++) {
        if (entries[i]->type != LIMINE_MEMMAP_USABLE) continue;
        for (u64 o = 0; o < entries[i]->length; o += PAGE_SIZE) {
            bitmap_clear(pmm_bitmap, (entries[i]->base + o) / PAGE_SIZE); /* clear the bitmap */
        }
    }

    dprintf("pmm_init(): PMM initialized successfully at address %lx!\n", (u64)pmm_bitmap);
}