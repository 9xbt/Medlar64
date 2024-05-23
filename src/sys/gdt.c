#include <sys/gdt.h>

gdt_table gdt = {
    {
        // Null segment
        0x0000000000000000,

        // 16-bit
        0x00009a000000ffff,
        0x000093000000ffff,

        // 32-bit
        0x00cf9a000000ffff,
        0x00cf93000000ffff,

        // 64-bit
        0x00af9b000000ffff,
        0x00af93000000ffff,

        // 64-bit user mode
        0x00affb000000ffff,
        0x00aff3000000ffff
    }
};

gdtr gdt_descriptor;

void gdt_init() {
    gdt_descriptor = (gdtr) {
        .size = (sizeof(u64) * 9) - 1,
        .offset = (u64)&gdt
    };

    asm volatile ("lgdt %0" :: "m"(gdt_descriptor) : "memory");
}