#include <mm/heap.h>
#include <mm/pmm.h>

__attribute__((no_sanitize("undefined")))
heap *heap_create() {
    heap* h = (heap*)HIGHER_HALF(pmm_alloc(1));
    h->block_head = (heap_block*)HIGHER_HALF(pmm_alloc(1));
    h->block_head->magic = HEAP_MAGIC;
    h->block_head->next = h->block_head->prev = h->block_head; /* funny code */
    h->block_head->size = 0;
    h->block_head->used = true;
    return h;
}