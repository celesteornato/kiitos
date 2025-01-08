#include "kiitklib.h"
#include "../kernel/kiitkio.h"
#include "../libc/kiitstdio.h"
void halt_and_catch_fire() {
  __asm__ volatile("cli; hlt");
  while (1)
    ;
}

__attribute__((noreturn))
void exception_handler() {
    __asm__ volatile ("cli; hlt"); // Completely hangs the computer
    while (1)
    ;
}

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

int kexec(char *cmd) {
  if (cmd[0] == 0)
    return 0;
  if (memcmp(cmd, "setfont 1", 10 * sizeof(char)) == 0) {
    k_setfont(&_binary_Solarize_12x29_psf_start);
    return 1;
  } else if (memcmp(cmd, "setfont 0", 10 * sizeof(char)) == 0) {
    k_setfont(&_binary_powerline_font_psf_start);
    return 2;
  } else if (memcmp(cmd, "ls", 2 * sizeof(char)) == 0) {
    return 3;
  } else if (memcmp(cmd, "clear", 6 * sizeof(char)) == 0) {
    return 4;
  } else if (memcmp(cmd, "reboot", 7 * sizeof(char)) == 0) {
    volatile char a = 0;
    volatile char b = 5;
    b /= a;
    return 5;
  } else if (memcmp(cmd, "getinfo 0", 8 * sizeof(char)) == 0) {
    return 7;
  }
  return 6;
}
