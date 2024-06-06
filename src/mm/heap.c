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

__attribute__((no_sanitize("undefined")))
void *heap_alloc(heap *h, u64 n) {
    u64 pages = DIV_CEILING(sizeof(heap_block) + n, PAGE_SIZE);
    heap_block *block = (heap_block*)HIGHER_HALF(pmm_alloc(pages));
    block->magic = HEAP_MAGIC;
    block->size = n;
    block->used = true;
    block->prev = h->block_head->prev;
    block->next = h->block_head;
    h->block_head->prev->next = block;
    h->block_head->prev = block;
    return (void*)block + sizeof(heap_block);
}