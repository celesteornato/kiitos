#include <arch/x86/asm_functions.h>
#include <arch/x86/asm_paging.h>
#include <basic/fbio.h>
#include <limine.h>
#include <memory/limine_map_requests.h>
#include <memory/paging.h>
#include <stddef.h>
#include <stdint.h>

enum : size_t {
    NODE_PER_LAYER = 512, // size of pml4t, of each pdpt in each pml4, etc
    PAGE_SIZE = 4096,
    LIMINE_OFFSET = 0xFFFF800000000000,
    PAGE_MASK = 0x000FFFFFFFFFFFFF000,
};

size_t pml4 = 0;
size_t hhdm_offs = 0;
static _Atomic size_t page_idx = 0;

static size_t *pml_to_vaddr(size_t pml)
{
    if (hhdm_offs == 0)
    {
        hhdm_offs = hhdm_request.response->offset;
    }
    return (size_t *)(((pml & PAGE_MASK) >> 12U) + hhdm_offs);
}

static size_t vaddr_to_pml(void *vaddr)
{
    if (hhdm_offs == 0)
    {
        hhdm_offs = hhdm_request.response->offset;
    }
    size_t h_addr = (size_t)vaddr - ((size_t)vaddr > hhdm_offs ? hhdm_offs : 0);
    return (h_addr << 12U) & PAGE_MASK;
}

static inline size_t align(size_t addr, size_t alignment) { return addr - (addr % alignment); }

/* returns a 4096k-aligned page during limine phase
** Limine guarantees that all higher-half pages are mapped.
*/
static void *linear_get_page(void)
{
    const size_t offs = hhdm_request.response->offset + 0x1000;

    char *addr = (char *)align(offs + (PAGE_SIZE * page_idx), 4096);

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
static size_t set_table(size_t table[], size_t index, size_t flags)
{
    if (!(table[index] & PRESENT))
    {
        void *page = linear_get_page();
        table[index] = vaddr_to_pml(page);
    }

    table[index] |= flags | PRESENT;
    return table[index];
}

void map_page(size_t physaddr, size_t virtualaddr, uint16_t flags)
{
    size_t pml4_idx = (virtualaddr >> 39U) & 0x1FFU;
    size_t pml3_idx = (virtualaddr >> 30U) & 0x1FFU;
    size_t pml2_idx = (virtualaddr >> 21U) & 0x1FFU;
    size_t pml1_idx = (virtualaddr >> 12U) & 0x1FFU;

    if (!(pml4 & PRESENT))
    {
        pml4 = vaddr_to_pml(linear_get_page()) | flags | PRESENT;
    }

    size_t pml3 = set_table(pml_to_vaddr(pml4), pml4_idx, flags);
    size_t pml2 = set_table(pml_to_vaddr(pml3), pml3_idx, flags);
    size_t pml1 = set_table(pml_to_vaddr(pml2), pml2_idx, flags);

    pml_to_vaddr(pml1)[pml1_idx] = (physaddr << 12U) | flags | PRESENT;
}
size_t get_physaddr(size_t virtualaddr)
{
    size_t pml4_idx = (virtualaddr >> 39U) & 0x1FFU;
    size_t pml3_idx = (virtualaddr >> 30U) & 0x1FFU;
    size_t pml2_idx = (virtualaddr >> 21U) & 0x1FFU;
    size_t pml1_idx = (virtualaddr >> 12U) & 0x1FFU;

    size_t pml3 = pml_to_vaddr(pml4)[pml4_idx];
    size_t pml2 = pml_to_vaddr(pml3)[pml3_idx];
    size_t pml1 = pml_to_vaddr(pml2)[pml2_idx];

    return (pml_to_vaddr(pml1)[pml1_idx] & PAGE_MASK) >> 12U;
}

void limine_remap(void *addr)
{
    size_t addrscalar = (size_t)addr;
    if (hhdm_offs == 0)
    {
        hhdm_offs = hhdm_request.response->offset;
    }

    if (addrscalar >= hhdm_offs)
    {
        map_page(addrscalar - hhdm_offs, addrscalar, RW);
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

    map_page(kern_addr_phys, kern_addr_virt, RW);

    if (get_physaddr(kern_addr_virt) != kern_addr_phys)
    {
        k_puts("!!!!!"); // Not printing, => the kernel is in the right place in our new pages
    }

    struct limine_memmap_response *mmr = memmap_request.response;
    for (size_t i = 0; i < mmr->entry_count; ++i)
    {
        switch (mmr->entries[i]->type)
        {
        case LIMINE_MEMMAP_USABLE:
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            size_t base = mmr->entries[i]->base;
            map_page(base, base, RW); // Those should be identity mappings
            break;
        default:
            break;
        }
    }

    // Finally, we load in the new pages.
    change_cr3((((pml4 - hhdm_offs) << 12U) & PAGE_MASK) | PRESENT | RW);
    // Code doesn't run past this line!!!!
}
