#include "amd64/interrupts/handlers.h"

#include "amd64/debug/logging.h"
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
void except_fatal(void)
{
    __asm__ volatile("call %P0; iretq" ::"i"(death));
}
