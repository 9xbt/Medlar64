#include <mm/kmalloc.h>
#include <mm/heap.h>

heap *kernel_heap;

void kheap_init() {
    kernel_heap = heap_create();
}

void *kmalloc(usize n) {
    void *ptr = heap_alloc(kernel_heap, n);
    return ptr;
}

void kfree(void *ptr) {
    /* todo: implement this */
}

void *krealloc(void *ptr, usize n) {
    /* todo: implement this too */
}