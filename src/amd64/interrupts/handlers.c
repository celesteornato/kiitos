#include <stdint.h>

struct [[gnu::packed]] register_info {
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rip;
};

[[gnu::naked]]
void common_exception(void)
{
    __asm__ volatile("call \n"
                     "iretq");
}
