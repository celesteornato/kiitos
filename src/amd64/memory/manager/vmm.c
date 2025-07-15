/*
 * For the entirety of this file, uintptr_t will mostly refer to a paging entry of the form
 * (physaddr << 12) | flags.
 */
#include "amd64/memory/manager/vmm.h"
#include "amd64/debug/logging.h"
#include "amd64/memory/manager/hhdm_setup.h"
#include "fun/colors.h"
#include "limine.h"
#include <stddef.h>
#include <stdint.h>

[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_memmap_request memmap_request = {.id = LIMINE_MEMMAP_REQUEST,
                                                               .revision = 0};
[[gnu::used, gnu::section(".limine_requests")]]
static volatile struct limine_kernel_address_request kern_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

static uintptr_t *pml4 = nullptr; // This will be given a page at runtime

static void switch_cr3(void)
{
    uintptr_t cr3 = hhdm_phys(pml4) | PRESENT | RDWR;
    __asm__ volatile("mov %0, %%cr3" ::"r"(cr3));
}

void vmm_init(void)
{
    pml4 = hhdm_get_page();
    uintptr_t kern_add_p = kern_request.response->physical_base;
    uintptr_t kern_add_v = kern_request.response->virtual_base;

    struct limine_memmap_response *mmr = memmap_request.response;

    for (uint64_t i = 0; i < mmr->entry_count; ++i)
    {
        size_t len = mmr->entries[i]->length;

        if (mmr->entries[i]->type == LIMINE_MEMMAP_KERNEL_AND_MODULES)
        {
            hhdm_mmap_len(pml4, kern_add_p, kern_add_v, PRESENT | RDWR, 1 + (len / 4096));
            continue;
        }
    }

    putsf("Switching pagemap...", COLOR, RED, D_BLUE);
    switch_cr3();
    while (true)
    {
    }
}
