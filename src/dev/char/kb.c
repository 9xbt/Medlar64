#include <dev/char/serial.h>
#include <dev/char/kb.h>
#include <lib/printf.h>
#include <dev/lapic.h>
#include <lib/libc.h>
#include <sys/idt.h>
#include <sys/io.h>

u8 kb_key_pressed = false;
u8 kb_char = '\0';
u8 kb_caps = false;
u8 kb_ctrl = false;
u8 kb_shift = false;

void kb_handler(regs* r) {
    (void)r;

    u8 key = inb(0x60);
    if (!(key & 0x80)) {
        switch (key) {
            case 0x2a:
                kb_shift = true;
                break;
            case 0x36:
                kb_shift = true;
                break;
            case 0x1d:
                kb_ctrl = true;
                break;
            case 0x3a:
                kb_caps = !kb_caps;
                break;
            default:
                kb_key_pressed = true;
                if (kb_shift) kb_char = kb_map_keys_shift[key];
                else if (kb_caps) kb_char = kb_map_keys_caps[key];
                else kb_char = kb_map_keys[key];
                break;
        }
    } else {
        switch (key) {
            case 0xaa:
                kb_shift = false;
                break;
            case 0xb6:
                kb_shift = false;
                break;
            case 0x9d:
                kb_ctrl = false;
                break;
        }
    }

    lapic_eoi();
}

char kb_get_char() {
    if (kb_key_pressed) {
        kb_key_pressed = false;
        return kb_char;
    } else {
        return '\0';
    }
}

void kb_get_string(char* buf, usize n) {
    memset(buf, 0, n);
    usize pos = 0;

    for (;;) {
        asm volatile ("hlt");

        switch(kb_char) {
            case '\0':
                break;

            case '\n':
                printf("\n");
                kb_char = 0;
                return;

            case '\b':
                if (pos <= 0)
                    break;
            
                printf("%c \b", kb_char);

                pos--;
                buf[pos] = 0;
                break;

            default:
                if (kb_ctrl && (kb_char == 'l' || kb_char == 'L')) {
                    printf("\033[2J\033[H");
                    
                    memset(buf, 0, n);
                    kb_char = 0;
                    return;
                }

                if (pos >= n - 1)
                    break;

                printf("%c", kb_char);

                buf[pos] = kb_char;
                pos++;
                break;
        }

        kb_char = 0;
    }
}

void kb_init() {
    irq_register(1, kb_handler);
    dprintf("ps/2: keyboard initialized\n");
}