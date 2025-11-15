#include "amd64/memory/manager/hhdm_setup.h"
#include "amd64/framebuffer/logging.h"
#include "amd64/memory/manager/pmm.h"
#include "amd64/memory/manager/vmm.h"
#include "fun/colors.h"
#include "limine.h"
#include "prologue/basefuncs.h"
#include <stddef.h>
#include <stdint.h>

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_hhdm_request hhdm_request = {.id = LIMINE_HHDM_REQUEST,
                                                           .revision = 0};

static size_t offset = 0;

/* Translates an virtaddress to its physical counterpart while the hhdm is active */
uintptr_t hhdm_phys(const void *addr)
{
    if (offset == 0)
    {
        offset = hhdm_request.response->offset;
    }

    if ((uintptr_t)addr >= offset)
    {
        return (uintptr_t)addr - offset;
    }

    // virtual addresses below offset are identity-mapped
    return (uintptr_t)addr;
}

/* Translates a physaddress to its virtual counterpart while the hhdm is active */
void *hhdm_virt(uintptr_t addr)
{
    if (offset == 0)
    {
        offset = hhdm_request.response->offset;
    }
    uintptr_t unaligned = (addr + offset);
    return (void *)(unaligned - (unaligned % 4096));
}

/* While the hhdm is active, get a new zeroed page from the pmm */
void *hhdm_get_page(void)
{
    uintptr_t paddr = 0;
    enum pmm_err err = pmm_alloc(&paddr);
    if (err != PMM_OK)
    {
        putsf("hhdm_get_page : pmm_alloc returned an error", LOG_COLOR, COLOR_RED);
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
    if (*pmle & PTE_PRESENT)
    {
        return;
    }

    void *pml_child = hhdm_get_page();
    if (pml_child == nullptr)
    {
        puts("Could not allocate phys memory!");
        __asm__ volatile("cli; hlt");
    }

    *pmle = hhdm_phys(pml_child) | flags;
}

/* We take pml_n at the index indicated by the vaddr, make it point to a physical page
 * if it doesn't * have one yet, and set pml_(n-1) to point to the corresponding virtual address  */
void hhdm_mmap(uintptr_t pml4[static 1], uintptr_t physaddr, uintptr_t vaddr, uint64_t flags)
{
    uintptr_t physaddr_aligned = physaddr - (physaddr % 4096);

    size_t pml4_idx = (vaddr >> 39U) & 0x1FFU;
    size_t pml3_idx = (vaddr >> 30U) & 0x1FFU;
    size_t pml2_idx = (vaddr >> 21U) & 0x1FFU;
    size_t pml1_idx = (vaddr >> 12U) & 0x1FFU;

    setup_page_layer(&pml4[pml4_idx], flags);
    uintptr_t *pml3 = hhdm_virt(pml4[pml4_idx]);
    pml3[511] = hhdm_phys(pml3) | flags;

    setup_page_layer(&pml3[pml3_idx], flags);
    uintptr_t *pml2 = hhdm_virt(pml3[pml3_idx]);
    pml2[511] = hhdm_phys(pml2) | flags;

    setup_page_layer(&pml2[pml2_idx], flags);
    uintptr_t *pml1 = hhdm_virt(pml2[pml2_idx]);
    pml1[511] = hhdm_phys(pml1) | flags;

    pml1[pml1_idx] = physaddr_aligned | flags;
}

void hhdm_mmap_len(uintptr_t pml4[static 1], uintptr_t paddr, uintptr_t vaddr, size_t flags,
                   size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        hhdm_mmap(pml4, paddr + (i * 4096), vaddr + (i * 4096), flags);
    }
}
