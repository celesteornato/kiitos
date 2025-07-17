#ifndef NVME_CONTROLS_H_
#define NVME_CONTROLS_H_

#include <stdint.h>

enum nvme_controls_error { NVME_CONTROLS_OK, NVME_CONTROLS_NOT_FOUND };

[[nodiscard]]
enum nvme_controls_error find_nvme_baddr(uintptr_t *out);

#endif // NVME_CONTROLS_H_
