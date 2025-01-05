#include "kiitlibc/kiitcolor.h"
#include "kiitlibc/kiitstdio.h"
#include "limine.h"
#include <stddef.h>
#include <stdint.h>

#define LONG_ASS_STR                                                           \
  "I'd just like to interject for a moment. What you're refering to as "       \
  "Kiitos, is in fact, KiitCoreUtils/Kiitos, or as I've recently taken to "    \
  "calling it, "                                                               \
  "KiitCoreUtils plus Kiitos. Kiitos is not an operating system unto itself, " \
  "but rather "                                                                \
  "another free component of a fully functioning KiitCoreUtils system made "   \
  "useful by "                                                                 \
  "the KiitCoreUtils corelibs, shell utilities and vital system components "   \
  "comprising "                                                                \
  "a full OS as defined by POSIX. Many computer users run a modified version " \
  "of the KiitCoreUtils system every day, without realizing it. Through a "    \
  "peculiar "                                                                  \
  "turn of events, the version of KiitCoreUtils which is widely used today "   \
  "is often "                                                                  \
  "called Kiitos, and many of its users are not aware that it is basically "   \
  "the KiitCoreUtils system, developed by the KiitCoreUtils Project. There "   \
  "really is a Kiitos, "                                                       \
  "and these people are using it, but it is just a part of the system they "   \
  "use. Kiitos is the kernel: the program in the system that allocates the "   \
  "machine's resources to the other programs that you run. The kernel is an "  \
  "essential part of an operating system, but useless by itself; it can only " \
  "function in the context of a complete operating system. Kiitos is "         \
  "normally "                                                                  \
  "used in combination with the KiitCoreUtils operating system: the whole "    \
  "system is "                                                                 \
  "basically KiitCoreUtils with Kiitos added, or KiitCoreUtils/Kiitos. All "   \
  "the so-called Kiitos "                                                      \
  "distributions are really distributions of KiitCoreUtils/Kiitos! I'd just "  \
  "like to "                                                                   \
  "interject for a moment. What you're refering to as Kiitos, is in fact, "    \
  "KiitCoreUtils/Kiitos, or as I've recently taken to calling it, "            \
  "KiitCoreUtils plus Kiitos. Kiitos "                                         \
  "is not an operating system unto itself, but rather another free component " \
  "of a fully functioning KiitCoreUtils system made useful by the "            \
  "KiitCoreUtils corelibs, shell "                                             \
  "utilities and vital system components comprising a full OS as defined by "  \
  "POSIX. Many computer users run a modified version of the KiitCoreUtils "    \
  "system every "                                                              \
  "day, without realizing it. Through a peculiar turn of events, the version " \
  "of KiitCoreUtils which is widely used today is often called Kiitos, and "   \
  "many of its "                                                               \
  "users are not aware that it is basically the KiitCoreUtils system, "        \
  "developed by the "                                                          \
  "KiitCoreUtils Project. There really is a Kiitos, and these people are "     \
  "using it, but "                                                             \
  "it is just a part of the system they use. Kiitos is the kernel: the "       \
  "program in the system that allocates the machine's resources to the other " \
  "programs that you run. The kernel is an essential part of an operating "    \
  "system, but useless by itself; it can only function in the context of a "   \
  "complete operating system. Kiitos is normally used in combination with "    \
  "the "                                                                       \
  "KiitCoreUtils operating system: the whole system is basically "             \
  "KiitCoreUtils with Kiitos added, "                                          \
  "or KiitCoreUtils/Kiitos. All the so-called Kiitos distributions are "       \
  "really                                                                    " \
  "  distributions of KiitCoreUtils/Kiitos!"

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

void cool_colors(struct limine_framebuffer *fb) {
  union color clr = {.value = RED};
  for (clr.rgb.g = 0; clr.rgb.g < 0xff; ++clr.rgb.g)
    k_puts(LONG_ASS_STR, 0, 0, 0xffffff - clr.value, clr.value, fb);
  for (clr.rgb.r = 0xff; clr.rgb.r > 0x0; --clr.rgb.r)
    k_puts(LONG_ASS_STR, 0, 0, 0xffffff - clr.value, clr.value, fb);
  for (clr.rgb.b = 0; clr.rgb.b < 0xff; ++clr.rgb.b)
    k_puts(LONG_ASS_STR, 0, 0, 0xffffff - clr.value, clr.value, fb);
  for (clr.rgb.g = 0xff; clr.rgb.g > 0x0; --clr.rgb.g)
    k_puts(LONG_ASS_STR, 0, 0, 0xffffff - clr.value, clr.value, fb);
  for (clr.rgb.r = 0; clr.rgb.r < 0xff; ++clr.rgb.r)
    k_puts(LONG_ASS_STR, 0, 0, 0xffffff - clr.value, clr.value, fb);
  for (clr.rgb.b = 0xff; clr.rgb.b > 0x0; --clr.rgb.b)
    k_puts(LONG_ASS_STR, 0, 0, 0xffffff - clr.value, clr.value, fb);
}

void kmain(void) {
  if (!LIMINE_BASE_REVISION_SUPPORTED)
    hcf();

  if (framebuffer_request.response == NULL ||
      framebuffer_request.response->framebuffer_count < 1)
    hcf();
  struct limine_framebuffer *framebuffer =
      framebuffer_request.response->framebuffers[0];

  init_stdout(0, 0, framebuffer);
  while(1)
    cool_colors(framebuffer);

  hcf();
}
