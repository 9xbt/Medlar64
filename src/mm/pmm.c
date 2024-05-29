#include <limine.h>
#include <mm/pmm.h>
#include <mm/bitmap.h>
#include <lib/libc.h>
#include <dev/char/serial.h>

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

struct limine_memmap_response* pmm_memmap = NULL;

u8* pmm_bitmap = NULL;
u64 pmm_last_page = 0;
u64 pmm_used_pages = 0;
u64 pmm_page_count = 0;

void pmm_init() {
    /* TODO: maybe use largest all memmap entries if possible */
    pmm_memmap = memmap_request.response;
    struct limine_memmap_entry** entries = pmm_memmap->entries;

    u64 higher_address = 0;
    u64 top_address = 0;

    for (u64 i = 0; i < pmm_memmap->entry_count; i++) {
        if (entries[i]->type != LIMINE_MEMMAP_USABLE) continue; /* ignore unusable memmaps */
        top_address = entries[i]->base + entries[i]->length;
        if (top_address > higher_address)
            higher_address = top_address;
    }

    pmm_page_count = higher_address / PAGE_SIZE;
    u64 bitmap_size = ALIGN_UP(pmm_page_count / 8, PAGE_SIZE);

    for (u64 i = 0; i < pmm_memmap->entry_count; i++) {
        if (entries[i]->type != LIMINE_MEMMAP_USABLE || entries[i]->length < bitmap_size) continue;
        /* we found a usuable memmap! use it! */
        pmm_bitmap = (u8*)HIGHER_HALF(entries[i]->base); /* use the higher half of memory */
        /* since we're storing the bitmap at the start of the memmap, subtract */
        /* the size of the bitmap to the memmap entry */
        entries[i]->base += bitmap_size;
        entries[i]->length -= bitmap_size;
        break;
    }

    for (u64 i = 0; i < pmm_memmap->entry_count; i++) {
        if (entries[i]->type != LIMINE_MEMMAP_USABLE) continue;
        for (u64 o = 0; o < entries[i]->length; o += PAGE_SIZE)
            bitmap_clear(pmm_bitmap, (entries[i]->base + o) / PAGE_SIZE); /* clear the bitmap */
    }

    dprintf("pmm_init(): PMM initialized successfully at address %lx!\n", (u64)pmm_bitmap);
}

u64 pmm_find_pages(u64 n) {
    u64 pages = 0;
    u64 first_page = pmm_last_page;

    while (pages < n) {
        if (pmm_last_page == pmm_page_count) {
            dprintf("pmm_find_pages(): Memory exhausted!\n");
            return 0;
        }

        if (!bitmap_get(pmm_bitmap, pmm_last_page + pages)) {
            /* we found a free page! */
            pages++;
            if (pages == n) {
                /* we found enough pages! */
                for (u64 i = 0; i < pages; i++) {
                    /* set the bits in the bitmap to tell the pages are used */
                    bitmap_set(pmm_bitmap, pmm_last_page + i);
                }
                
                pmm_last_page += pages;
                return first_page; /* return the start of the pages, note that this isn't actually the address to the page */
            }
        }
        else {
            /* no free pages yet... */
            pmm_last_page += (pages == 0 ? 1 : pages); /* increase by one even if we didn't find a page */
            first_page = pmm_last_page; /* increase the address of the first found page for later use*/
            pages = 0;
        }
    }
    return 0;
}

void* pmm_alloc(usize n) {
    u64 pages = pmm_find_pages(n);
    if (pages == 0) {
        pmm_last_page = 0;
        pages = pmm_find_pages(n);
        if (pages == 0) {
            dprintf("pmm_alloc(): Failed to allocate %ld pages: Memory exhausted\n", n);
            return NULL;
        }
    }

    u64 physical_addr = pages * PAGE_SIZE;
    return (void*)(physical_addr);
}

void pmm_free(void* p, usize n) {
    u64 page = (u64)p / PAGE_SIZE;
    for (u64 i = 0; i < n; i++)
        bitmap_clear(pmm_bitmap, page + i);
}