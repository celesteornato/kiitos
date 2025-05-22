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

struct cr3_pml4_descriptor pml4r = {.flags = RW};
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
    const size_t offs = LIMINE_OFFSET + 0xFFFF;

    char *addr = (char *)align_addr_start(offs + (PAGE_SIZE * page_idx), 4096);
    for (size_t i = 0; i < 4096; ++i)
    {
        addr[i] = 0;
    }
    page_idx++;
    return addr;
}

#define MAP_NEXT_LEVEL(curr_name, next_name)                                                       \
    do                                                                                             \
    {                                                                                              \
        struct next_name##e *page_off_ = get_zeroed_page_frame_addr();                             \
        (curr_name)->next_name##_addr = (size_t)(page_off_ - LIMINE_OFFSET);                       \
        (curr_name)->flags |= PRESENT | flags;                                                     \
    } while (0)

/*
** Find index at level nth PML, mark PML as present, make address point to some arbitrary cool
*higher-half address that's 4096k-aligned, go into (n-1)th PML. When PT is reached (PML0), we
*make the entry mapped to physaddr
*/
void map_page(size_t physaddr, size_t virtualaddr, uint16_t flags)
{
    union linear_address addr = {.value = virtualaddr};

    if (!(pml4r.flags & PRESENT))
    {
        struct pml4e *page_off = get_zeroed_page_frame_addr();
        pml4r.pml4_addr = (size_t)(page_off - LIMINE_OFFSET);
        pml4r.flags |= PRESENT | flags;
    }

    // Writing this without ptr arithmetic makes it less clear
    struct pml4e *pml4 = (struct pml4e *)(pml4r.pml4_addr + LIMINE_OFFSET + addr.pml4_idx);

    if (!(pml4->flags & PRESENT))
    {
        MAP_NEXT_LEVEL(pml4, pml3);
    }

    struct pml3e *pml3 = (struct pml3e *)(pml4->pml3_addr + LIMINE_OFFSET + addr.pml3_idx);
    if (!(pml3->flags & PRESENT))
    {
        MAP_NEXT_LEVEL(pml3, pml2);
    }

    struct pml2e *pml2 = (struct pml2e *)(pml3->pml2_addr + LIMINE_OFFSET + addr.pml2_idx);
    if (!(pml2->flags & PRESENT))
    {
        MAP_NEXT_LEVEL(pml2, pml1);
    }
    struct pml1e *pml1 = (struct pml1e *)(pml2->pml1_addr + LIMINE_OFFSET + addr.pml1_idx);
    *pml1 = (struct pml1e){.flags = flags | PRESENT, .page_addr = physaddr};
}

void limine_remap(void *addr)
{
    size_t addrscalar = (size_t)addr;
    if (addrscalar >= LIMINE_OFFSET)
    {
        map_page(addrscalar - LIMINE_OFFSET, addrscalar, PRESENT | RW);
        return;
    }
    map_page(addrscalar, addrscalar, PRESENT | RW);
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
    map_page(kern_addr_phys, kern_addr_virt, PRESENT | RW);

    struct limine_memmap_response *mmr = memmap_request.response;
    for (size_t i = 0; i < mmr->entry_count; ++i)
    {
        // nb : entries[i]->base is physical.
        switch (mmr->entries[i]->type)
        {
        case LIMINE_MEMMAP_USABLE:
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            size_t base = mmr->entries[i]->base;
            map_page(base, base + LIMINE_OFFSET, PRESENT | RW);
            break;
        default:
            break;
        }
    }
    // Finally, we load in the new pages.
    change_cr3(pml4r);
}
