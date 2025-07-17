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

[[noreturn]]
static void death(void)
{
    putsf("Oop, seems like you've died!", COLOR, RED | BLUE, D_BLUE);
    while (true)
    {
    }
}

[[noreturn]]
static void pf_handler(uintptr_t vaddr)
{
    putsf("#PF! At %", UNUM, 16, vaddr);
    death();
}

void except_fatal(void)
{
    __asm__ volatile("call %P0; iretq" ::"i"(death));
}

void page_fault(void)
{
    __asm__ volatile("pop %%rdi;" // Consume the errcode, because we don't use it yet
                     "mov %%cr2, %%rdi;"
                     "call %P0;"
                     "iretq"
                     :
                     : "i"(pf_handler));
}
