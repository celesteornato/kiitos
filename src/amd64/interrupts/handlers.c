#include "amd64/interrupts/handlers.h"

#include "amd64/debug/logging.h"
#include "amd64/memory/manager/hhdm_setup.h"
#include "amd64/memory/manager/vmm.h"
#include "fun/colors.h"
#include <stdint.h>

struct [[gnu::packed]] register_info {
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rip;
};

static void death(void)
{
    putsf("Oop, seems like you've died!", COLOR, RED | BLUE, D_BLUE);
    while (true)
    {
    }
}
static void pf_handler(uintptr_t vaddr)
{
    putsf("#PF! Allocating %...", NUM, 16, vaddr);
    uintptr_t cr3 = 0;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    uintptr_t *pml4 = hhdm_virt(cr3);
    hhdm_mmap(pml4, hhdm_phys((void *)vaddr), vaddr, PRESENT | RDWR | GLOBAL);
}

void except_fatal(void)
{
    __asm__ volatile("call %P0; iretq" ::"i"(death));
}

void page_fault(void)
{
    __asm__ volatile("pop %%rdx;" // Consume the errcode, because we don't use it yet
                     "mov %%cr2, %%rdx;"
                     "call %P0;"
                     "iretq"
                     :
                     : "i"(pf_handler));
}
