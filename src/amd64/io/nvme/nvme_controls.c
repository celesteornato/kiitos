#include "amd64/io/nvme/nvme_controls.h"
#include "amd64/io/pci_io.h"
#include <stddef.h>

enum nvme_controls_error find_nvme_baddr(uintptr_t *out)
{
    struct pci_device_location_info nvme_info = {};
    if (pci_find_matching_dev(0x1, 0x8, &nvme_info) != PCI_IO_OK)
    {
        return NVME_CONTROLS_NOT_FOUND;
    }

    uint32_t bar0 = pci_h0_get_bar(nvme_info.dev_num, nvme_info.bus, 0);
    uint32_t bar1 = pci_h0_get_bar(nvme_info.dev_num, nvme_info.bus, 1);
    *out = (uintptr_t)(((uintptr_t)bar1 << 32U) | (bar0 & 0xFFFFFFF0U));
    return NVME_CONTROLS_OK;
}
