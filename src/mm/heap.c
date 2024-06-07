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

void heap_free(heap *h, void *ptr) {
    heap_block *block = (heap_block*)(ptr - sizeof(heap_block));

    if (block->magic != HEAP_MAGIC) {
        dprintf("heap: bad magic at %lx\n", ptr);
        dprintf("magic: %x", block->magic);
        return;
    }

    block->prev->next = block->next;
    block->next->prev = block->prev;
    u64 pages = DIV_CEILING(sizeof(heap_block) + block->size, PAGE_SIZE);
    pmm_free(PHYSICAL(ptr - sizeof(heap_block)), pages);
}

void* heap_realloc(heap* h, void* ptr, u64 n) {
    heap_block* block = (heap_block*)(ptr - sizeof(heap_block));
    if (block->magic != HEAP_MAGIC) {
        dprintf("heap: bad magic at %lx.\n", ptr);
        return NULL;
    }
    void* new_ptr = heap_alloc(h, n);
    if (!new_ptr) return NULL;
    memcpy(new_ptr, ptr, block->size);
    heap_free(h, ptr);
    return new_ptr;
}