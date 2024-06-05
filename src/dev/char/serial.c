#include <dev/char/serial.h>

void serial_write_char(char c, void* extra) {
    (void)extra;
    outb(0xe9, c);
}

void dprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfctprintf(serial_write_char, NULL, format, args);
    va_end(args);

    //va_start(args, format);
    //vprintf(format, args);
    //va_end(args);
}