#include <dev/char/kb.h>
#include <mm/kmalloc.h>
#include <lib/printf.h>
#include <lib/libc.h>

void shell_launch() {
    char* buf = kmalloc(1024);

    for (;;) {
        printf("medlar > ");

        kb_get_string(buf, 1024);

        if (!strcmp(buf, "")) {
        } else if (!strcmp(buf, "help")) {
            printf("commands: help, clear\n");
        } else if (!strcmp(buf, "clear")) {
            printf("\033[2J\033[H");
        } else {
            printf("%s: command not found\n", buf);
        }
    }
}