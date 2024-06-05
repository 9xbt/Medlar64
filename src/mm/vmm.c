#include <limine.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <lib/libc.h>

struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

pagemap* vmm_pm;

__attribute__((no_sanitize("undefined")))
void vmm_init() {
    vmm_pm = (pagemap*)HIGHER_HALF(pmm_alloc(1));
    memset(vmm_pm, 0, PAGE_SIZE);

    vmm_switch_pm(vmm_pm);

    dprintf("vmm: initialized at address %lx\n", (u64)vmm_pm);
}

__attribute__((no_sanitize("undefined")))
void vmm_switch_pm(pagemap* pm) {
    asm volatile("mov %0, %%cr3" :: "r"((u64)PHYSICAL(pm)) : "memory");
}