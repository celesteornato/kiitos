#ifndef LIMINE_MAP_REQUESTS_H_
#define LIMINE_MAP_REQUESTS_H_

#include <limine.h>

__attribute__((used, section(".limine_requests"))) static volatile struct limine_memmap_request
    memmap_request = {.id = LIMINE_MEMMAP_REQUEST, .revision = 0};

__attribute__((
    used, section(".limine_requests"))) static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0};

__attribute__((used,
               section(".limine_requests"))) static volatile struct limine_kernel_address_request
    kern_add_request = {.id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

#endif // LIMINE_MAP_REQUESTS_H_
