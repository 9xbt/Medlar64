#pragma once

#include <types.h>

void kheap_init();

void *kmalloc(usize n);
void  kfree(void *ptr);
void *krealloc(void *ptr, usize n);