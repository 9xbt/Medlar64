/*
 * CREDITS: astrido
 * https://github.com/asterd-og/ZanOS/
 */

#pragma once

#include <types.h>
#include <lib/printf.h>
#include <sys/io.h>

void serial_write_char(char c, void* extra);
void dprintf(const char* fmt, ...);