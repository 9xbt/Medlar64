#include <limine.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <lib/libc.h>

struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

uptr* vmm_kernel_pm;

__attribute__((no_sanitize("undefined")))
void vmm_init() {
    vmm_kernel_pm = (uptr*)HIGHER_HALF(pmm_alloc(1));
    memset(vmm_kernel_pm, 0, PAGE_SIZE);

    dprintf("vmm: initialized at address %lx\n", (u64)vmm_kernel_pm);
}