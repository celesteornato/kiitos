#include "amd64/interrupts/handlers.h"
#include "amd64/framebuffer/logging.h"
#include "fun/colors.h"
#include <stdint.h>

struct [[gnu::packed]] register_info {
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rip;
};

static struct register_info get_registers(void)
{
    struct register_info inf;
    __asm__ volatile("mov %%rbp, %0;"
                     "mov %%rsp, %1;"
                     "leaq 0(%%rip), %2;" // Hack, %rip cannot be addressed directly
                     : "=g"(inf.rbp), "=g"(inf.rsp), "=a"(inf.rip));
    return inf;
}

static void print_reg(void)
{
    struct register_info regs = get_registers();
    putsf("Registers:\n"
          "\trbp: 0x%\n"
          "\trsp: 0x%\n"
          "\trip: 0x%",
          LOG_UNUM, 16, regs.rbp, regs.rsp, regs.rip);
}

[[noreturn]]
static void death(void)
{
    putsf("Oop, seems like you've died!", LOG_COLOR, COLOR_RED | COLOR_BLUE, COLOR_D_BLUE);
    print_reg();
    while (true)
    {
    }
}

[[noreturn]]
static void pf_handler(uintptr_t vaddr)
{
    putsf("#PF! At %", LOG_UNUM, 16, vaddr);
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
