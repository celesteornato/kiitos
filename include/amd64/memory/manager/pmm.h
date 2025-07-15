#ifndef PMM_H_
#define PMM_H_

#include <stdint.h>
enum pmm_err {
    PMM_OK,
    PMM_NO_AVAIL,
    PMM_ALREADY_FREE,
};

/* Allocates a page-alligned physical address.
 * Returns an error and works with out parameters. */
[[nodiscard]]
enum pmm_err pmm_alloc(uintptr_t *out);

/* Frees a phys address if it is allocated, returns PMM_ALREADY_FREE otherwise.  */
[[nodiscard]]
enum pmm_err pmm_free(uintptr_t);

#endif // PMM_H_
