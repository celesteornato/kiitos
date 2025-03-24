#include "basic/fbio.h"
#include "init/gdt.h"
#include "interrupts/idt.h"
#include "limine.h"
#include "prologue/basefuncs.h"
#include "prologue/prologue.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LONGSTR                                                                \
  "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp" \
  "pppppppppppppppppppppppp"

union color {
  uint32_t hex;
  struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
  };
};

static void hcf(void) {
  for (;;) {
    __asm__("hlt");
  }
}

static inline uint64_t pixel_per_row(struct limine_framebuffer *fb) {
  return fb->pitch * 8 / fb->bpp;
}

void kmain(void) {
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  // Ensure we got a framebuffer.
  if (framebuffer_request.response == NULL ||
      framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }

  struct limine_framebuffer *framebuffer =
      framebuffer_request.response->framebuffers[0];

  uint32_t *fb_ptr = framebuffer->address;

  __asm__("cli");
  gdt_init();
  __asm__("sti");
  const size_t ppr = pixel_per_row(framebuffer);
  k_puts("hmwwo!", fb_ptr, ppr, 0, 0, 20, 0, framebuffer->width / 2,
         framebuffer->height);
  idt_init();
  k_puts(LONGSTR LONGSTR LONGSTR LONGSTR, fb_ptr, ppr, 0, 0, 20, 0,
         framebuffer->width / 2, framebuffer->height);
  hcf();
}
