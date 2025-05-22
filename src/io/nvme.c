#include <arch/x86/asm_functions.h>
#include <basic/fbio.h>
#include <io/nvme.h>
#include <io/pci.h>
#include <memory/paging.h>
#include <stdint.h>

void *init_nvme(void)
{
    struct pci_device_location_info nvme = pci_find_matching_dev(0x01, 0x08);

    if (nvme.devnum == 255)
    {
        k_print("\tCould not find NVMe!\n", URG2 | URG1);
        hcf();
    }

    k_print("\tFound NVMe at device 0x", 0);
    k_printd_base(nvme.devnum, 16);
    k_puts(" ");

    // We will be doing pointer arithmetic, so they are promoted to uint64_t
    uint64_t bar0 = pci_get_bar(nvme.devnum, nvme.bus, 0);
    uint64_t bar1 = pci_get_bar(nvme.devnum, nvme.bus, 1);

    void *phys_addr = (void *)((bar1 << 32U) | (bar0 & 0xFFFFFFF0U));

    return phys_addr;
}
