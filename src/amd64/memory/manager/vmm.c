/*
 * For the entirety of this file, uintptr_t will mostly refer to a paging entry of the form
 * (physaddr << 12) | flags.
 */
#include "amd64/memory/manager/vmm.h"
#include "amd64/debug/logging.h"
#include "amd64/memory/manager/pmm.h"
#include "fun/colors.h"
#include "limine.h"
#include "prologue/basefuncs.h" // provides memset
#include <stddef.h>
#include <stdint.h>

enum page_table_flags : uint64_t {
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

static uintptr_t *pml4 = nullptr; // This will be given a page at runtime

/* Translates an virtaddress to its physical counterpart while the hhdm is active */
static uintptr_t hhdm_phys(const void *addr)
{
    size_t offset = hhdm_request.response->offset;

    if ((uintptr_t)addr >= offset)
    {
        return (uintptr_t)addr - offset;
    }

    // virtual addresses below offset are identity-mapped
    return (uintptr_t)addr;
}

/* Translates a physaddress to its virtual counterpart while the hhdm is active */
static void *hhdm_virt(uintptr_t addr)
{
    size_t offset = hhdm_request.response->offset;
    uintptr_t unaligned = (addr + offset);
    return (void *)(unaligned - (unaligned % 4096));
}

/* While the hhdm is active, get a new zeroed page from the pmm */
static void *get_new_page(void)
{
    uintptr_t paddr = 0;
    enum pmm_err err = pmm_alloc(&paddr);
    if (err != PMM_OK)
    {
        putsf("vmm.c@get_new_page : pmm_alloc returned an error", COLOR, RED);
        return nullptr;
    }
    void *addr = hhdm_virt(paddr);
    memset(addr, 0, 512 * sizeof(uintptr_t));
    return addr;
}

static void setup_page_layer(uintptr_t *pmle, size_t flags)
{
    /* We only pass in elements of an initially-zeroed-out array, so this flag
     * cannot be wrongfully set */
    if (*pmle & PRESENT)
    {
        return;
    }

    putsf("Setting up new pml at address %", NUM, 16, pmle);

    void *pml_child = get_new_page();
    if (pml_child == nullptr)
    {
        puts("Could not allocate phys memory!");
        __asm__ volatile("cli; hlt");
    }

    *pmle = hhdm_phys(pml_child) | flags;
    putsf("Setup pml at address 0x%", NUM, 16, *pmle);
}

/* Will only work while limine's hhdm is active.
 * We take pml_n at the index indicated by the vaddr, make it point to a physical page
 * if it doesn't * have one yet, and set pml_(n-1) to point to the corresponding virtual address  */
static void hhdm_mmap(uintptr_t physaddr, uintptr_t vaddr, uint64_t flags)
{
    uintptr_t physaddr_aligned = physaddr - (physaddr % 4096);

    size_t pml4_idx = (vaddr >> 39U) & 0x1FFU;
    size_t pml3_idx = (vaddr >> 30U) & 0x1FFU;
    size_t pml2_idx = (vaddr >> 21U) & 0x1FFU;
    size_t pml1_idx = (vaddr >> 12U) & 0x1FFU;

    putsf("Mapping phys % -> virt % ", NUM, 16, physaddr, vaddr);

    setup_page_layer(&pml4[pml4_idx], flags);
    uintptr_t *pml3 = hhdm_virt(pml4[pml4_idx]);

    setup_page_layer(&pml3[pml3_idx], flags);
    uintptr_t *pml2 = hhdm_virt(pml3[pml3_idx]);

    setup_page_layer(&pml2[pml2_idx], flags);
    uintptr_t *pml1 = hhdm_virt(pml2[pml2_idx]);

    pml1[pml1_idx] = physaddr_aligned | flags;
}

/* mmaps a certain number of adjacent pages */
static void mmap_len(uintptr_t paddr, uintptr_t vaddr, size_t flags, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        hhdm_mmap(paddr + (i * 4096), vaddr + (i * 4096), flags);
    }
}

/* BUG: This is the point where things break. Changing the value of cr3 stalls the cpu.
 * As far as I can tell, this means the kernel code wasn't correctly remapped, but I have
 * no idea what makes it incorrect */
static void switch_cr3(void)
{
    uintptr_t cr3 = hhdm_phys(pml4) | PRESENT | RDWR;
    __asm__ volatile("mov %0, %%cr3" ::"r"(cr3));
}

void vmm_init(void)
{
    pml4 = get_new_page();
    uintptr_t kern_add_p = kern_request.response->physical_base;
    uintptr_t kern_add_v = kern_request.response->virtual_base;

    struct limine_memmap_response *mmr = memmap_request.response;

    for (uint64_t i = 0; i < mmr->entry_count; ++i)
    {
        size_t len = mmr->entries[i]->length;

        if (mmr->entries[i]->type != LIMINE_MEMMAP_KERNEL_AND_MODULES)
        {
            // For now, we only want to map the kernel to see whether the processor stalls
            continue;
        }
        putsf("Kernel - length : %", NUM, 10, 1 + (len / 4096));
        mmap_len(kern_add_p, kern_add_v, PRESENT | RDWR, 1 + (len / 4096));
        putc('\n');
    }

    putsf("Switching pagemap...", COLOR, RED, D_BLUE);
    switch_cr3();
    // If we get there, we win!
    while (true)
    {
    }
}
