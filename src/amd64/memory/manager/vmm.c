/*
 * For the entirety of this file, uintptr_t will mostly refer to a paging entry of the form
 * (physaddr << 12) | flags.
 */
#include "amd64/memory/manager/vmm.h"
#include "amd64/debug/logging.h"
#include "amd64/memory/manager/pmm.h"
#include "limine.h"
#include <stddef.h>
#include <stdint.h>

enum vmm_flags : uint64_t {
    PRESENT = 1 << 0,
    RDWR = 1 << 1,
    USER = 1 << 2,
    PWT = 1 << 3,
    PCD = 1 << 4,
    PAT = 1 << 7,
    GLOBAL = 1 << 8,
    NX = 1LL << 63,
};

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_memmap_request memmap_request = {.id = LIMINE_MEMMAP_REQUEST,
                                                               .revision = 0};

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_hhdm_request hhdm_request = {.id = LIMINE_HHDM_REQUEST,
                                                           .revision = 0};
[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_kernel_address_request kern_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

static constexpr size_t TABLE_SIZE = 512;
alignas(4096) static uintptr_t pml4[TABLE_SIZE] = {};

static uintptr_t hhdm_phys(const void *addr)
{
    size_t offset = hhdm_request.response->offset;
    if ((uintptr_t)addr >= offset)
    {
        return (uintptr_t)addr - offset;
    }
    return (uintptr_t)addr;
}
static void *hhdm_virt(uintptr_t addr)
{
    size_t offset = hhdm_request.response->offset;
    uintptr_t unaligned = (addr + offset);
    return (void *)(unaligned - (unaligned % 4096));
}

static void switch_cr3(void)
{
    uintptr_t cr3 = hhdm_phys(pml4) | PRESENT | RDWR;
    __asm__ volatile("mov %0, %%cr3" ::"r"(cr3));
}

static void setup_page_layer(uintptr_t *pmle, size_t flags)
{
    if (*pmle & PRESENT)
    {
        return;
    }

    uintptr_t child = 0;
    if (pmm_alloc(&child) != PMM_OK)
    {
        puts("Could not allocate phys memory!");
    }
    *pmle |= child | flags;
}

static void mmap(uintptr_t physaddr, const void *vaddr, uint64_t flags)
{
    uintptr_t physaddr_aligned = physaddr - (physaddr % 4096);
    uintptr_t add = (uintptr_t)vaddr;
    size_t pml4_idx = (add >> 39U) & 0x1FFU;
    size_t pml3_idx = (add >> 30U) & 0x1FFU;
    size_t pml2_idx = (add >> 21U) & 0x1FFU;
    size_t pml1_idx = (add >> 12U) & 0x1FFU;

    putsf("Mapping phys %   :   % ", NUM, 16, physaddr, vaddr);
    putsf("Indices: %, %, %, %", NUM, 10, pml4_idx, pml3_idx, pml2_idx, pml1_idx);

    putsf("Setting up pml4 at address %", NUM, 16, pml4);
    setup_page_layer(&pml4[pml4_idx], flags);
    uintptr_t *pml3 = hhdm_virt(pml4[pml4_idx]);

    putsf("Setting up pml3 at address %", NUM, 16, pml3);
    setup_page_layer(&pml3[pml3_idx], flags);
    pml3[511] = hhdm_phys(pml3);
    uintptr_t *pml2 = hhdm_virt(pml3[pml3_idx]);

    putsf("Setting up pml2 at address %", NUM, 16, pml2);
    setup_page_layer(&pml2[pml2_idx], flags);
    pml2[511] = hhdm_phys(pml2);
    uintptr_t *pml1 = hhdm_virt(pml2[pml2_idx]);

    putsf("Setting up pml1 at address %", NUM, 16, pml1);
    pml1[511] = hhdm_phys(pml1);
    pml1[pml1_idx] = physaddr_aligned | flags;
}

void remap(const void *addr)
{
    mmap(hhdm_phys(addr), addr, PRESENT | RDWR);
}

static void mmap_len(uintptr_t paddr, void *vaddr, size_t flags, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        mmap(paddr + (i * 4096), (void *)((uintptr_t)vaddr + (i * 4096)), flags);
    }
}

void vmm_init(void)
{
    uintptr_t kerna_p = kern_request.response->physical_base;
    void *kerna_v = (void *)kern_request.response->virtual_base;

    struct limine_memmap_response *mmr = memmap_request.response;

    for (uint64_t i = 0; i < mmr->entry_count; ++i)
    {
        size_t len = mmr->entries[i]->length;

        switch (mmr->entries[i]->type)
        {
        case LIMINE_MEMMAP_USABLE:
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            size_t base = mmr->entries[i]->base;
            putsf("Usable/bootloader - length : %", NUM, 10, len);
            // Those should be identity mappings
            mmap_len(base, (void *)base, PRESENT | RDWR, 1 + (len / 4096));
            putc('\n');
            break;
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            putsf("Kernel - length : %", NUM, 10, len);
            mmap_len(kerna_p, kerna_v, PRESENT | RDWR, 1 + (len / 4096));
            putc('\n');
            break;
        default:
            break;
        }
    }

    puts("Switching pagemap...");
    switch_cr3();
    while (true)
    {
    }
}
