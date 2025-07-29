#include "amd64/framebuffer/logging.h" // I *think* this one is portable
#include "fun/art.h"
#include "fun/colors.h"
#include "limine.h"
#include "limits.h"
#include <stddef.h>
#include <stdint.h>

#ifdef KIITOS_BUILD_AMD64
#include "amd64/amd64_init.h"
#endif

[[gnu::used, gnu::section(".limine_requests")]] static volatile LIMINE_BASE_REVISION(3)

    [[gnu::used,
      gnu::section(".limine_requests")]] static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

static inline uint64_t pixel_per_row(struct limine_framebuffer *fb)
{
    return (CHAR_BIT * fb->pitch) / fb->bpp;
}

[[noreturn]] void hcf(void)
{
    while (true)
    {
    }
}

[[noreturn]] void kmain(void)
{
    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        hcf();
    }
    struct limine_framebuffer_response *volatile response = framebuffer_request.response;
    if (response == nullptr || response->framebuffer_count < 1)
    {
        hcf();
    }

    struct limine_framebuffer *framebuffer = response->framebuffers[0];
    uint32_t *fb_ptr = framebuffer->address;

    logging_config(fb_ptr, framebuffer->width, framebuffer->height, pixel_per_row(framebuffer));
    change_fb_colors(COLOR_WHITE, COLOR_D_BLUE);

    putc('\f');
    puts(foomp_art);
    puts("Welcome to KiitOS/3!");
    putc('\n');

    putsf("Initialising arch-specific components - ", COLOR | NOBREAK, COLOR_GREEN, COLOR_D_BLUE);
    arch_init();

    hcf();
}
