#include "amd64/io/pci_io.h"
#include "amd64/io/port_communication.h"
#include <stdint.h>

enum pci_ports : uint16_t { PCI_CONFIG_IN = 0x0CF8, PCI_CONFIG_OUT = 0x0CFC };

static constexpr uint8_t PCI_MAX_DEV_NUM = 64;
static constexpr uint8_t PCI_MAX_BUS = 255;

struct pci_config_message {
    uint8_t offset;
    uint8_t id; // 3 func number + 5 dev number
    uint8_t bus_number;
    uint8_t flags; // 7 reserved + 1 enable
};

static uint32_t pci_get_dev_word(struct pci_config_message msg)
{
    union {
        uint32_t address;
        struct pci_config_message m;
    } tmp = {.m = msg};

    outl(PCI_CONFIG_IN, tmp.address);

    return inl(PCI_CONFIG_OUT);
}

uint16_t pci_get_vendor(uint8_t dev_num, uint8_t bus)
{
    uint8_t id = (uint8_t)(dev_num << 3U);
    struct pci_config_message msg = {.offset = 0x0, .id = id, .bus_number = bus, .flags = 1 << 7};

    // Vendor is in the 16 first bits of a PCI header
    return pci_get_dev_word(msg) & 0xFFFFU;
}

uint8_t pci_get_header_type(uint8_t dev_num, uint8_t bus)
{
    uint8_t id = (uint8_t)(dev_num << 3U);
    struct pci_config_message msg = {.offset = 0xC, .id = id, .bus_number = bus, .flags = 1 << 7};

    // Header is in the 3rd byte of a PCI header at offset 0xC
    return (uint8_t)((pci_get_dev_word(msg) >> 16U) & 0xFFU);
}

uint32_t pci_h0_get_bar(uint8_t dev_num, uint8_t bus, uint8_t bar_num)
{
    uint8_t offset = (uint8_t)0x10U + (uint8_t)(bar_num * sizeof(uint32_t));
    uint8_t id = (uint8_t)(dev_num << 3U);
    struct pci_config_message msg = {
        .offset = offset, .id = id, .bus_number = bus, .flags = 1 << 7};

    return pci_get_dev_word(msg);
}

uint16_t pci_get_class(uint8_t dev_num, uint8_t bus)
{

    uint8_t id = (uint8_t)(dev_num << 3U);
    struct pci_config_message msg = {.offset = 0x8, .id = id, .bus_number = bus, .flags = 1 << 7};

    return (uint16_t)(pci_get_dev_word(msg) >> 16);
}

enum pci_io_err pci_find_matching_dev(uint8_t class, uint8_t subclass,
                                      struct pci_device_location_info *out)
{
    uint16_t EXPECTED_CL_SL = (uint16_t)((uint16_t)class << 8U) | subclass;
    for (uint8_t bus = 0; bus < PCI_MAX_BUS; ++bus)
    {
        for (uint8_t dev_num = 0; dev_num < PCI_MAX_DEV_NUM; ++dev_num)
        {
            if (pci_get_class(dev_num, bus) == EXPECTED_CL_SL)
            {
                *out = (struct pci_device_location_info){dev_num, bus};
                return PCI_IO_OK;
            }
        }
    }
    return PCI_IO_DEV_NOT_FOUND;
}
