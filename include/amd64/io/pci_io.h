#ifndef PCI_IO_H_
#define PCI_IO_H_

#include <stdint.h>

enum pci_io_err { PCI_IO_OK, PCI_IO_DEV_NOT_FOUND };

struct pci_device_location_info {
    uint8_t dev_num;
    uint8_t bus;
};

/* Outs the device number and bus of the first found device with a matching class and subclass */
[[nodiscard]]
enum pci_io_err pci_find_matching_dev(uint8_t class, uint8_t subclass,
                                      struct pci_device_location_info *out);

uint8_t pci_get_header_type(uint8_t dev_num, uint8_t bus);
uint16_t pci_get_vendor(uint8_t dev_num, uint8_t bus);
uint16_t pci_get_class(uint8_t dev_num, uint8_t bus);
uint32_t pci_h0_get_bar(uint8_t dev_num, uint8_t bus, uint8_t bar_num);

#endif // PCI_IO_H_
