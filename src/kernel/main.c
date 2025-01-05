#include "../limine.h"
#include "kiitkio.h"
#include "../userland/shell.h"
#include <stddef.h>
#include <stdint.h>

#define KIITOS_VERSION "0.0.1"

__attribute__((
    used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);

__attribute__((
    used,
    section(
        ".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((used,
               section(".limine_requests_"
                       "start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((
    used,
    section(
        ".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

void *memcpy(void *dest, const void *src, size_t n) {
  uint8_t *pdest = (uint8_t *)dest;
  const uint8_t *psrc = (const uint8_t *)src;

  for (size_t i = 0; i < n; i++) {
    pdest[i] = psrc[i];
  }

  return dest;
}

void *memset(void *s, int c, size_t n) {
  uint8_t *p = (uint8_t *)s;

  for (size_t i = 0; i < n; i++) {
    p[i] = (uint8_t)c;
  }

  return s;
}

void *memmove(void *dest, const void *src, size_t n) {
  uint8_t *pdest = (uint8_t *)dest;
  const uint8_t *psrc = (const uint8_t *)src;

  if (src > dest) {
    for (size_t i = 0; i < n; i++) {
      pdest[i] = psrc[i];
    }
  } else if (src < dest) {
    for (size_t i = n; i > 0; i--) {
      pdest[i - 1] = psrc[i - 1];
    }
  }

  return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *p1 = (const uint8_t *)s1;
  const uint8_t *p2 = (const uint8_t *)s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

static void hcf(void) {
  for (;;) {
    asm("hlt");
  }
}

void kmain(void) {
  if (!LIMINE_BASE_REVISION_SUPPORTED)
    hcf();

  if (framebuffer_request.response == NULL ||
      framebuffer_request.response->framebuffer_count < 1)
    hcf();
  asm("cli");

  struct limine_framebuffer *framebuffer =
      framebuffer_request.response->framebuffers[0];

  kcolor_fbuff(framebuffer, 0x000000);
  k_puts("Welcome to KiitOS!\n    Version: ", 0, 0, 0xffff00, 0x0, framebuffer);
  k_puts(KIITOS_VERSION, 14, 1, 0x00ff00, 0x0, framebuffer);

  drop_into_shell(out_new(framebuffer)); 

  hcf();
}