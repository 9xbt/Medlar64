/*
 * CREDITS: astrido
 * https://github.com/asterd-og/ZanOS/
 */

#include <dev/char/serial.h>

void serial_write_char(char c, void* extra) {
    (void)extra;
    outb(0xe9, c);
}

void dprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfctprintf(serial_write_char, NULL, fmt, args);
    va_end(args);
}