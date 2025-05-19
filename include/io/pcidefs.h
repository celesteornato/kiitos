#ifndef PCIDEFS_H_
#define PCIDEFS_H_

#include <stdint.h>
enum pci_ports : uint16_t { PCI_CONFIG_IN = 0x0CF8, PCI_CONFIG_OUT = 0x0CFC };

static const uint8_t PCI_MAX_DEV_NUM = 64;

struct pci_config_message {
    uint8_t offset;
    uint8_t func_number : 3;
    uint8_t dev_number : 5;
    uint8_t bus_number;
    uint8_t reserved : 7;
    uint8_t enable : 1;
};

#endif // PCIDEFS_H_
