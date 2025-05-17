#include <assert.h>
#include <basic/fbio.h>
#include <init/gdt.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <io/mouse.h>
#include <limine.h>
#include <misc/art.h>
#include <prologue/prologue.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Halt and catch fire
static void hcf(void)
{
    for (;;)
    {
        __asm__("hlt");
    }
}

static inline uint64_t pixel_per_row(struct limine_framebuffer *fb)
{
    return fb->pitch * 8 / fb->bpp;
}

void kmain(void)
{
    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        hcf();
    }

    // Ensure we got a framebuffer.
    struct limine_framebuffer_response *volatile response = framebuffer_request.response;

    if (response == NULL || response->framebuffer_count < 1)
    {
        hcf();
    }

    struct limine_framebuffer *framebuffer = response->framebuffers[0];

    uint32_t *fb_ptr = framebuffer->address;

    const size_t ppr = pixel_per_row(framebuffer);
    k_set_buff_settings(fb_ptr, ppr, framebuffer->width, framebuffer->height);

    k_paint(0x0e3d37);
    k_dbg_puts(ASCII_WELCOME);

    __asm__("cli");
    k_dbg_puts("\n\nInitialising CPU...");
    gdt_init();
    k_dbg_puts("\tGDT loaded!");
    idt_init();
    k_dbg_puts("\tIDT loaded!");
    pic_init();
    k_dbg_puts("\tPIC configured!");
    __asm__("sti");

    k_dbg_puts("\nInitialising PS/2 Mouse...");
    ps2_mouse_init();
    extern int clock;
    while (true)
    {
    }
}
