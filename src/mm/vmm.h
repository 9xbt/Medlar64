#pragma once

#include <types.h>

#define PTE_PRESENT 1ul
#define PTE_WRITABLE 2ul
#define PTE_USER 4ul
#define PTE_NX (1ul << 63)

#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_GET_ADDR(x) ((x) & PTE_ADDR_MASK)
#define PTE_GET_FLAGS(x) ((x) & ~PTE_ADDR_MASK)

typedef uptr pagemap;

extern symbol text_start_ld;
extern symbol text_end_ld;
extern symbol rodata_start_ld;
extern symbol rodata_end_ld;
extern symbol data_start_ld;
extern symbol data_end_ld;

void vmm_init();