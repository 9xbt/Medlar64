#include <mm/kmalloc.h>
#include <mm/heap.h>

heap *kernel_heap;

void kheap_init() {
    kernel_heap = heap_create();
    dprintf("heap: created kernel heap\n");
}

void *kmalloc(usize n) {
    return heap_alloc(kernel_heap, n);
}

void kfree(void *ptr) {
    heap_free(kernel_heap, ptr);
}

void *krealloc(void *ptr, usize n) {
    /* todo: implement this too */
}