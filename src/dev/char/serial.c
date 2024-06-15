#include <dev/char/serial.h>
#include <sys/io.h>

u16 serial_port = 0;

void serial_init() {
    outb(COM1 + 1, 0);
    outb(COM1 + 3, 0x80);
    outb(COM1, 0x03);
    outb(COM1 + 1, 0);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x1E);
    outb(COM1, 0xAE);

    if (inb(COM1) != 0xAE) {
        serial_port = QEMU;
    } else {
        serial_port = COM1;
        outb(COM1 + 4, 0x0F);
    }
    
    dprintf("serial: enabled port %x\n", serial_port);
}

int serial_is_bus_empty() {
    return inb(COM1 + 5) & 0x20;
}

void serial_write_char(char c) {
    while (serial_is_bus_empty() == 0);
    outb(COM1, c);
}

void serial_wrapper(char c, void* extra) {
    (void)extra;
    outb(serial_port, c);
}

void dprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfctprintf(serial_wrapper, NULL, format, args);
    va_end(args);

    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}