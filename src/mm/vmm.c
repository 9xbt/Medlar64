#include <limine.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <lib/libc.h>
#include <dev/char/serial.h>

struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

uptr* vmm_kernel_pm = NULL;

void vmm_init() {
    vmm_kernel_pm = (uptr*)HIGHER_HALF(pmm_alloc(1));
    memset(vmm_kernel_pm, 0, PAGE_SIZE);

    dprintf("vmm_init(): VMM initialized successfully! Page map located at %lx\n", (u64)vmm_kernel_pm);
}