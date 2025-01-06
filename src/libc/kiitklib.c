#include "kiitklib.h"
#include "../kernel/kiitkio.h"
__attribute__((noreturn)) void halt_and_catch_fire(void) {
  __asm__("cli");
  while (1)
    __asm__("hlt");
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
  if (memcmp(cmd, "setfont1", 8 * sizeof(char)) == 0) {
    k_setfont(&_binary_Solarize_12x29_psf_start);
    return 1;
  } else if (memcmp(cmd, "setfont0", 8 * sizeof(char)) == 0) {
    k_setfont(&_binary_powerline_font_psf_start);
    return 2;
  } else if (memcmp(cmd, "ls", 2 * sizeof(char)) == 0) {
    return 3;
  } else if (memcmp(cmd, "clear", 5 * sizeof(char)) == 0) {
    return 4;
  } else if (memcmp(cmd, "rbot", 4 * sizeof(char)) == 0) {
    *(long*)(0x1)=0xDeadBeef;
    return 5;
  }
  return 6;
}
