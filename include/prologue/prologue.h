#ifndef PROLOGUE_H_
#define PROLOGUE_H_

#include "limine.h"

// Formatter struggles with this part
__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(0)

    __attribute__((used,
                   section(".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((used, section(".limine_requests_"
                             "start"))) static volatile LIMINE_REQUESTS_START_MARKER

    __attribute__((used, section(".limine_requests_"
                                 "end"))) static volatile LIMINE_REQUESTS_END_MARKER

#endif // PROLOGUE_H_
