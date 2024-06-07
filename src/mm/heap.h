#pragma once

#include <types.h>

#define HEAP_MAGIC 0xdeadcafe

typedef struct heap_block { /* gcc likes doing the funny forcing me to specify the name twice */
    struct heap_block *next;
    struct heap_block *prev;
    bool used;
    u32 magic;
    u64 size;
} heap_block;

typedef struct {
    heap_block *block_head;
} heap;

heap *heap_create();

void *heap_alloc(heap *h, u64 n);
void  heap_free(heap* h, void* ptr);
void* heap_realloc(heap* h, void* ptr, u64 n);