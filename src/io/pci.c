#include <arch/x86/asm_io.h>
#include <io/pci.h>
#include <io/pcidefs.h>
#include <stdint.h>

uint32_t pci_get_dev_word(struct pci_config_message msg)
{
    // Fully defined type punning
    union {
        uint32_t address;
        struct pci_config_message m;
    } tmp = {.m = msg};

    outl(PCI_CONFIG_IN, tmp.address);

    return inl(PCI_CONFIG_OUT);
}

uint16_t pci_get_vendor(uint8_t dev_num)
{
    const struct pci_config_message msg = {
        .bus_number = 0,
        .dev_number = dev_num,
        .func_number = 0,
        .offset = 0x0,
        .reserved = 0,
        .enable = 1,
    };

    // Vendor is in the 16 first bits of a PCI header
    return pci_get_dev_word(msg) & 0xFFFFU;
}
uint8_t pci_get_header_type(uint8_t dev_num)
{
    const struct pci_config_message msg = {
        .bus_number = 0,
        .dev_number = dev_num,
        .func_number = 0,
        .offset = 0xC,
        .reserved = 0,
        .enable = 1,
    };

    // Header is in the 3rd byte of a PCI header at offset 0xC
    return (pci_get_dev_word(msg) >> 16U) & 0xFFU;
}

uint32_t pci_get_bar(uint8_t dev_num, uint8_t bar_num)
{
    const uint8_t BAR0_OFFSET = 0x10;
    const struct pci_config_message msg = {
        .bus_number = 0,
        .dev_number = dev_num,
        .func_number = 0,
        .offset = BAR0_OFFSET + (4 * bar_num),
        .reserved = 0,
        .enable = 1,
    };

    // Header is in the 3rd byte of a PCI header at offset 0xC
    return (pci_get_dev_word(msg) >> 16U) & 0xFFU;
}

uint16_t pci_get_class_full(uint8_t dev_num)
{
    const uint8_t CLASS_OFFSET = 0x8;
    const struct pci_config_message msg = {
        .bus_number = 0,
        .dev_number = dev_num,
        .func_number = 0,
        .offset = CLASS_OFFSET,
        .reserved = 0,
        .enable = 1,
    };

    // Class + subclass are concatenated in the upper 2 bytes of offset 0x8
    return (pci_get_dev_word(msg) >> 16U) & 0xFFFFU;
}

// NVMe devices are identified by class code 1 subclass 8
uint8_t pci_find_nvme_devnum(void)
{
    const uint16_t EXPECTED_CL_SL = 0x0801;
    for (uint8_t i = 0; i < PCI_MAX_DEV_NUM; ++i)
    {
        if (pci_get_class_full(i) == EXPECTED_CL_SL)
        {
            return i;
        }
    }
    return 255;
}
