#include "../libc/kiitklib.h"
#include "../limine.h"
#include "../userland/shell.h"
#include "gdt.h"
#include "kiitkio.h"
#include <stddef.h>

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

void kmain(void) {
  if (!LIMINE_BASE_REVISION_SUPPORTED)
    halt_and_catch_fire();

  if (framebuffer_request.response == NULL ||
      framebuffer_request.response->framebuffer_count < 1)
    halt_and_catch_fire();

  struct limine_framebuffer *framebuffer =
      framebuffer_request.response->framebuffers[0];

  kcolor_fbuff(framebuffer, 0x000000);
  struct out err1 = out_new(framebuffer);
  struct out otp1 = out_new(framebuffer);
  global_out = out_new(framebuffer);
  otp1.fg = 0xffff00;
  err1.fg = 0xff0000;
  prints("Welcome to KiitOS\n\tVersion: ", &otp1);
  otp1.fg = 0x00ff00;
  otp1.bg = 0xff00ff;
  prints(KIITOS_VERSION, &otp1);
  printc('\n', &otp1);
  otp1.fg = 0xffffff;
  otp1.bg = 0x000000;

  prints("Loading gdt...", &otp1);
  gdt_install();
  prints("\r[FINISHED] Loading gdt\n", &otp1);
  halt_and_catch_fire();

  prints("Dropping into shell...\n", &otp1);
  drop_into_shell(&otp1, &err1);
  prints("Shell exited!\n", &otp1);

  halt_and_catch_fire();
}
