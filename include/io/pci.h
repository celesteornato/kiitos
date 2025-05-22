#ifndef PCI_H_
#define PCI_H_

#include <io/pcidefs.h>
#include <stdint.h>

struct pci_device_location_info {
    uint8_t devnum;
    uint8_t bus;
};

uint32_t pci_get_dev_word(struct pci_config_message);

/* Gets the vendor for a specified PCI device number */
uint16_t pci_get_vendor(uint8_t dev_num, uint8_t bus);

/* Gets the header for a specified PCI device number */
uint8_t pci_get_header_type(uint8_t dev_num, uint8_t bus);

/*
 ** Gets the BAR for a specified PCI device number and bar number

 ** Be careful, dev_num MUST refer to a device with header_type 0
*/
uint32_t pci_get_bar(uint8_t dev_num, uint8_t bus, uint8_t bar_num);

/* Find the first PCI device to be classified as an NVMe, returns 255 if none found */
struct pci_device_location_info pci_find_matching_dev(uint8_t class, uint8_t subclass);

/* Gets class and subclass of a device_number concatenated, as present in the upper 16 bits of
  PCI offset 0x8 */
uint16_t pci_get_class_full(uint8_t dev_num, uint8_t bus);

#endif // PCI_H_
