#include "amd64/memory/manager/pmm.h"
#include <stddef.h>
#include <stdint.h>

#define countof(arr) (sizeof(arr) / sizeof(arr[1]))

static uint64_t bitmap[1000] = {};
static constexpr int page_size = 4096;
static constexpr int bits_per_row = sizeof(bitmap[0]) * 8;
static constexpr int offset = 0x8000;

enum pmm_err pmm_alloc(uintptr_t *out)
{
    for (size_t row = 0; row < (ptrdiff_t)countof(bitmap); ++row)
    {
        for (size_t bit = 0; bit < bits_per_row; ++bit)
        {
            if ((bitmap[row] & (1 << bit)))
            {
                continue;
            }
            bitmap[row] |= (1 << bit);
            *out = (((row * bits_per_row) + bit) * page_size) + offset;
            return PMM_OK;
        }
    }
    return PMM_NO_AVAIL;
}

enum pmm_err pmm_free(uintptr_t addr)
{
    // Safe conversion because bpr is at most 64
    size_t bit = (size_t)((addr - offset) % bits_per_row);
    size_t row = (addr - offset) / bits_per_row;

    if ((bitmap[row] & bit) == 0)
    {
        return PMM_ALREADY_FREE;
    }
    bitmap[row] &= ~bit;
    return PMM_OK;
}
