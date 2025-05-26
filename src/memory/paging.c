#include <arch/x86/asm_functions.h>
#include <arch/x86/asm_paging.h>
#include <limine.h>
#include <memory/limine_map_requests.h>
#include <memory/paging.h>

#include <stddef.h>
#include <stdint.h>

enum : size_t {
    NODE_PER_LAYER = 512, // size of pml4t, of each pdpt in each pml4, etc
    PAGE_SIZE = 4096,
    LIMINE_OFFSET = 0xffff800000000000,
};

size_t pml4r = 0;
static size_t page_idx = 0;

static inline size_t align_addr_start(size_t addr, size_t alignment)
{
    return addr - (addr % alignment);
}
/*
** returns a 4096k-aligned page during limine phase
** Limine guarantees that all higher-half pages are mapped.
*/
static void *get_zeroed_page_frame_addr(void)
{
    const size_t offs = hhdm_request.response->offset + 0xFFFF;

    char *addr = (char *)align_addr_start(offs + (PAGE_SIZE * page_idx), 4096);
    for (size_t i = 0; i < PAGE_SIZE; ++i)
    {
        addr[i] = 0;
    }
    page_idx++;
    return addr;
}

/*
** Puts a pml into memory with the right flags if it isn't already, in both case returns the
** virtual address in which the PML's child array is allocated
**
** Only to be used while limine's paging is active.
*/
static size_t *update_pml(size_t *p, uint16_t flags)
{
    const size_t PML_TO_VADDR_MASK = 0x000FFFFFFFFFF000U;
    const size_t off = hhdm_request.response->offset;
    if (!(*p & PRESENT))
    {
        size_t *page_off = get_zeroed_page_frame_addr();
        *p = ((size_t)(page_off - off) << 12U) | flags | PRESENT;
        return page_off;
    }
    size_t page_haddr = ((size_t)p & PML_TO_VADDR_MASK) >> 12U;
    return (size_t *)(page_haddr + off);
}

/*
** Find index at level nth PML, mark PML as present, make address point to some arbitrary cool
*higher-half address that's 4096k-aligned, go into (n-1)th PML. When PT is reached (PML0), we
*make the entry mapped to physaddr.
*
* Also, the type system goes to die here. Do not mind it.
*/
void map_page(size_t physaddr, size_t virtualaddr, uint16_t flags)
{
    union linear_address addr = {.value = virtualaddr};
    size_t *pml4t = update_pml(&pml4r, flags);
    pml4t[addr.pml4_idx] = 0;
    size_t *pml3t = update_pml(&pml4t[addr.pml4_idx], flags);
    pml3t[addr.pml3_idx] = 0;
    size_t *pml2t = update_pml(&pml3t[addr.pml3_idx], flags);
    pml2t[addr.pml2_idx] = 0;
    size_t *pml1t = update_pml(&pml2t[addr.pml2_idx], flags);
    pml1t[addr.pml1_idx] = (physaddr << 12U) | PRESENT | flags;
}

void limine_remap(void *addr)
{
    size_t addrscalar = (size_t)addr;
    const size_t offs = hhdm_request.response->offset;
    if (addrscalar >= offs)
    {
        map_page(addrscalar - offs, addrscalar, RW);
        return;
    }
    map_page(addrscalar, addrscalar, RW);
}

/* Need to remap:
 * Kernel -> kern_addr_request
 * Framebuffer -> fb_addr from main();

 * Usable -> memmap_request
 * Bootloader reclaimable -> memmap_request
 * */
void paging_init(void)
{

    size_t kern_addr_phys = kern_add_request.response->physical_base;
    size_t kern_addr_virt = kern_add_request.response->virtual_base;
    const size_t offs = hhdm_request.response->offset;

    map_page(kern_addr_phys, kern_addr_virt, RW);

    struct limine_memmap_response *mmr = memmap_request.response;
    for (size_t i = 0; i < mmr->entry_count; ++i)
    {
        // nb : entries[i]->base is physical.
        switch (mmr->entries[i]->type)
        {
        case LIMINE_MEMMAP_USABLE:
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            size_t base = mmr->entries[i]->base;
            map_page(base, base + offs, RW);
            break;
        default:
            break;
        }
    }
    // Finally, we load in the new pages.
    change_cr3(pml4r);
}
