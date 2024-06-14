#include <types.h>
#include <limine.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/heap.h>
#include <mm/kmalloc.h>
#include <sys/io.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <dev/pit.h>
#include <dev/lapic.h>
#include <dev/char/serial.h>
#include <lib/libc.h>
#include <lib/printf.h>
#include <acpi/acpi.h>
#include <acpi/madt.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

// See specification for further info.

LIMINE_BASE_REVISION(1)

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, in C, they should
// NOT be made "static".

struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0  
};

u64 hhdm_offset;

struct limine_framebuffer *framebuffer = NULL;
struct flanterm_context *ft_ctx = NULL;

void hcf(void) {
    for (;;)
        asm volatile ("hlt");
}

void putchar_(char c) {
    char str[] = {c};
    flanterm_write(ft_ctx, str, 1);
}

void mubsan_write_char(char c, void* extra) {
    (void)extra;
    char msg[] = {c, 0};
    dprintf(msg);
}

void mubsan_log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfctprintf(mubsan_write_char, NULL, fmt, args);
    va_end(args);
    
    hcf();
}

void _start(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false)
        hcf();

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1)
        hcf();

    hhdm_offset = hhdm_request.response->offset;
    framebuffer = framebuffer_request.response->framebuffers[0];

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
        NULL, &fg,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0
    );

    gdt_init();
    idt_init();
    pmm_init();
    vmm_init();

    printf("\nWelcome to \033[1;36mAlpine\033[0m!\n\n");

    kheap_init();
    acpi_init();
    madt_init();
    lapic_init();
    pit_init();

    hcf();
}