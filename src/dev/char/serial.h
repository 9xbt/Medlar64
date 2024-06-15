#pragma once

#include <types.h>
#include <lib/printf.h>
#include <sys/io.h>

#define QEMU 0xE9
#define COM1 0x3F8

void serial_init();
void dprintf(const char* fmt, ...);