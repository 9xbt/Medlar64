#pragma once

#include <types.h>

#define HEAP_MAGIC 0xdeadcafe

typedef struct {
    bool used;
    u32 magic;
    u64 size;

    struct heap_block *next;
    struct heap_block *prev;
} heap_block;

typedef struct {
    heap_block *block_head;
} heap;

heap *heap_create();