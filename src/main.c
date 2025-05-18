#include <arch/x86/asm_functions.h>
#include <basic/fbio.h>
#include <init/gdt.h>
#include <interrupts/helpers/syscall.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <io/keyboard.h>
#include <io/mouse.h>
#include <limine.h>
#include <misc/art.h>
#include <misc/colours.h>
#include <prologue/prologue.h>
#include <stddef.h>
#include <stdint.h>

static inline uint64_t pixel_per_row(struct limine_framebuffer *fb)
{
    const int bits_per_bytes = 8;
    return bits_per_bytes * fb->pitch / fb->bpp;
}

[[noreturn]] void kmain(void)
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

    k_paint(BG);
    k_puts(ASCII_WELCOME);

    interrupt_disable();

    k_puts("\n\nInitialising CPU...");
    gdt_init();
    k_puts("\tGDT loaded!");
    idt_init();
    k_puts("\tIDT loaded!");
    pic_init();
    k_puts("\tPIC configured!");

    interrupt_enable();

    k_puts("\nInitialising PS/2 Mouse...");
    ps2_mouse_init();

    while (1)
    {
        syscall_dbg(FBWRITE, syscall_dbg(KBINP, 0, 0, 0, 0), 1, 0, 0);
    }
}
