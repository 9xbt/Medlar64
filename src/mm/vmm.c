#include <dev/char/serial.h>
#include <lib/libc.h>
#include <limine.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

extern symbol text_start_ld;
extern symbol text_end_ld;
extern symbol rodata_start_ld;
extern symbol rodata_end_ld;
extern symbol data_start_ld;
extern symbol data_end_ld;
struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

pagemap *vmm_kernel_pm;

__attribute__((no_sanitize("undefined")))
void vmm_switch_pm(pagemap *pm) {
    asm volatile("mov %0, %%cr3" ::"r"((u64)PHYSICAL(pm)) : "memory");
}

__attribute__((no_sanitize("undefined")))
void vmm_init() {
    vmm_kernel_pm = (pagemap *)HIGHER_HALF(pmm_alloc(1));
    memset(vmm_kernel_pm, 0, PAGE_SIZE);

    uptr phys_base = kernel_address_request.response->physical_base;
    uptr virt_base = kernel_address_request.response->virtual_base;

    /* im declaring start addresses here even if i set the text uptr to this just
    * for consistency*/
    uptr text_start = ALIGN_DOWN((uptr)text_start_ld, PAGE_SIZE);
    uptr text_end = ALIGN_UP((uptr)text_end_ld, PAGE_SIZE);
    uptr rodata_start = ALIGN_DOWN((uptr)rodata_start_ld, PAGE_SIZE);
    uptr rodata_end = ALIGN_UP((uptr)rodata_end_ld, PAGE_SIZE);
    uptr data_start = ALIGN_DOWN((uptr)data_start_ld, PAGE_SIZE);
    uptr data_end = ALIGN_UP((uptr)data_end_ld, PAGE_SIZE);

    for (uptr text = text_start; text < text_end; text += PAGE_SIZE)
      vmm_map(vmm_kernel_pm, text, text - virt_base + phys_base, PTE_PRESENT);
    for (uptr rodata = rodata_start; rodata < rodata_end; rodata += PAGE_SIZE)
      vmm_map(vmm_kernel_pm, rodata, rodata - virt_base + phys_base,
              PTE_PRESENT | PTE_NX);
    for (uptr data = data_start; data < data_end; data += PAGE_SIZE)
      vmm_map(vmm_kernel_pm, data, data - virt_base + phys_base,
              PTE_PRESENT | PTE_WRITABLE | PTE_NX);
    for (uptr addr = 0; addr < 0x100000000;
        addr += PAGE_SIZE) { /* map the first 4gib */
      vmm_map(vmm_kernel_pm, addr, addr, PTE_PRESENT | PTE_WRITABLE);
      vmm_map(vmm_kernel_pm, (uptr)HIGHER_HALF(addr), addr,
              PTE_PRESENT | PTE_WRITABLE);
    }

    dprintf("vmm: kernel page map located at %lx", (u64)vmm_kernel_pm);
}

__attribute__((no_sanitize("undefined")))
uptr *vmm_get_next_lvl(uptr *lvl, uptr entry, u64 flags, bool alloc) {
    if (lvl[entry] & PTE_PRESENT) return HIGHER_HALF(PTE_GET_ADDR(lvl[entry]));
    if (!alloc) return NULL;

    uptr *pml = (uptr *)HIGHER_HALF(pmm_alloc(1));
    memset(pml, 0, PAGE_SIZE);
    lvl[entry] = (uptr)PHYSICAL(pml) | flags;
    return pml;
}

__attribute__((no_sanitize("undefined")))
void vmm_map(pagemap *pm, uptr virt, uptr phys, u64 flags) {
    uptr pml4i = (virt >> 39) & 0x1ff;
    uptr pml3i = (virt >> 30) & 0x1ff;
    uptr pml2i = (virt >> 21) & 0x1ff;
    uptr pml1i = (virt >> 12) & 0x1ff;

    uptr *pml3 = vmm_get_next_lvl(pm, pml4i, PTE_PRESENT | PTE_WRITABLE, true);
    uptr *pml2 = vmm_get_next_lvl(pml3, pml3i, PTE_PRESENT | PTE_WRITABLE, true);
    uptr *pml1 = vmm_get_next_lvl(pml2, pml2i, PTE_PRESENT | PTE_WRITABLE, true);

    pml1[pml1i] = phys | flags;
}
