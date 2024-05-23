#include <types.h>
#include <limine.h>
#include <sys/io.h>
#include <sys/gdt.h>
#include <lib/printf.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

// See specification for further info.

LIMINE_BASE_REVISION(1)

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, in C, they should
// NOT be made "static".

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

struct limine_framebuffer *framebuffer = NULL;
struct flanterm_context *ft_ctx = NULL;

void putchar_(char c) {
    char str[] = {c};
    flanterm_write(ft_ctx, str, 1);
}

static void hcf(void) {
    for (;;)
        asm volatile ("hlt");
}

void _start(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false)
        hcf();

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1)
        hcf();

    framebuffer = framebuffer_request.response->framebuffers[0];

    u32 bg = 0x000000;
    u32 fg = 0xffffff;

    ft_ctx = flanterm_fb_init(
        NULL,
        NULL,
        framebuffer->address, framebuffer->width,
        framebuffer->height, framebuffer->pitch,
        framebuffer->red_mask_size, framebuffer->red_mask_shift,
        framebuffer->green_mask_size, framebuffer->green_mask_shift,
        framebuffer->blue_mask_size, framebuffer->blue_mask_shift,
        NULL,
        NULL, NULL,
        &bg, &fg,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0
    );

    printf("Welcome to \033[92mAlpine\033[0m!\n\n");

    gdt_init();

    hcf();
}