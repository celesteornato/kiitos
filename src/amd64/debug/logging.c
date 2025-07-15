#include "amd64/debug/logging.h"
#include "fun/colors.h"
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

// PC Screen Font (v2) header
struct psf_font {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t length;
    uint32_t glyph_size;
    uint32_t height;
    uint32_t width;
};

static struct {
    volatile uint32_t *fb;
    size_t width;
    size_t height;
    size_t ppr;

    size_t x;
    size_t y;

    uint32_t fg;
    uint32_t bg;
} fb_info;

extern const struct psf_font _binary_src_assets_spleen_psf_start; // NOLINT;

const struct psf_font *default_font = &_binary_src_assets_spleen_psf_start;

static constexpr uint32_t DEFAULT_BG = D_BLUE;
static constexpr uint32_t DEFAULT_FG = WHITE;

void logging_config(volatile uint32_t *fb, size_t width, size_t height, size_t ppr)
{
    fb_info = (typeof(fb_info)){.fb = fb,
                                .width = width,
                                .height = height,
                                .ppr = ppr,
                                .x = 0,
                                .y = 0,
                                .fg = DEFAULT_FG,
                                .bg = DEFAULT_BG};
}

static void internal_putc(char c, volatile uint32_t fb[static 1], size_t ppr, size_t x_off,
                          size_t y_off, uint32_t fg, uint32_t bg)
{
    const uint8_t *glyph = (const uint8_t *)default_font + default_font->header_size +
                           (default_font->glyph_size * (ptrdiff_t)c);

    // PSF2 standard imposes unsigned for height/width, so unsigned array indices it is
    size_t offset = (y_off * default_font->height * ppr) + (x_off * default_font->width);

    // (w + 7) / 8 fixes alignment issues, allegedly
    uint32_t bytesperline = default_font->width / 8;

    /* Algorithm:
     *  For each line, we, for each byte, mask each bit
     * */
    for (size_t y = 0; y < default_font->height; ++y)
    {
        for (size_t x = 0; x < default_font->width; ++x)
        {
            const uint8_t current_byte = glyph[x / 8];
            constexpr const uint8_t highest_bit = 0x80U;
            const uint8_t current_bitmask = highest_bit >> (x & 7U);

            fb[offset + x] = (current_byte & current_bitmask) ? fg : bg;
        }
        glyph += bytesperline;
        offset += ppr;
    }
}

void clear_fb(uint32_t bg)
{
    size_t h = fb_info.height;
    size_t w = fb_info.width;

    volatile uint32_t *fb = fb_info.fb;
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            fb[x + (y * w)] = bg;
        }
    }
}

static void clear_line(void)
{
    size_t w = fb_info.width;
    size_t y = fb_info.y * default_font->height;
    size_t max_y = (fb_info.y + 1) * default_font->height;

    uint32_t bg = fb_info.bg;
    volatile uint32_t *fb = fb_info.fb;
    for (; y < fb_info.height && y < max_y; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            fb[x + (y * w)] = bg;
        }
    }
}

/* Yes, you can write this in a pretty way with just modulo operations,
 * but this is easier to understand */
static void clamp_fbinfo(void)
{
    if (fb_info.x >= fb_info.width / default_font->width)
    {
        fb_info.x = 0;
        fb_info.y += 1;
        clear_line();
    }
    if (fb_info.x < 0)
    {
        fb_info.x = fb_info.width - 1;
        fb_info.y -= 1;
        clear_line();
    }

    if (fb_info.y >= fb_info.height / default_font->height)
    {
        fb_info.y = 0;
        fb_info.x = 0;
        clear_line();
    }
    if (fb_info.y < 0)
    {
        fb_info.y = fb_info.height - 1;
        clear_line();
    }
}

void change_fb_colors(uint32_t fg, uint32_t bg)
{
    fb_info.fg = fg;
    fb_info.bg = bg;
}

void putc(char c)
{
    switch (c)
    {
    case '\n':
        fb_info.x = 0;
        fb_info.y += 1;
        clear_line();
        break;
    case '\t':
        fb_info.x += 4;
        break;
    case '\b':
        fb_info.x -= 1;
        break;
    case '\r':
        fb_info.x = 0;
        break;
    case '\f':
        fb_info.x = 0;
        fb_info.y = 0;
        clear_fb(fb_info.bg);
        break;
    default:
        internal_putc(c, fb_info.fb, fb_info.ppr, fb_info.x++, fb_info.y, fb_info.fg, fb_info.bg);
    }
    clamp_fbinfo();
}

/* We could go without indices by just dereferencing and incrementing the pointer.
 * But come on, do you just *want* to be hostile to people reading your code ? */
void puts(const char *str)
{
    for (int i = 0; str[i] != '\0'; ++i)
    {
        putc(str[i]);
    }
    putc('\n');
}

uintptr_t get_fb_address(void)
{
    return (uintptr_t)fb_info.fb;
}

static void print_number(uint64_t n, uint32_t radix)
{
    if (radix == 0)
    {
        putsf(" RADIX 0! ", NOBREAK | COLOR, RED, D_BLUE);
    }
    if (n == 0)
    {
        putc('0');
        return;
    }
    if (n < 0)
    {
        putc('-');
        n = -n;
    }

    char to_print[32] = {};
    ptrdiff_t idx = 0;

    while (n > 0)
    {
        if ((n % radix) < 10)
        {
            to_print[idx++] = (char)('0' + (n % radix));
        }
        else
        {
            to_print[idx++] = (char)('A' + (n % radix) - 10);
        }
        n /= radix;
    }
    idx -= 1;
    while (idx >= 0)
    {
        putc(to_print[idx--]);
    }
}

void putsf(const char *str, uint32_t flags, ...)
{
    va_list args;
    va_start(args, 2);

    uint32_t old_fg = fb_info.fg;
    uint32_t old_bg = fb_info.bg;

    if (flags & COLOR)
    {

        fb_info.fg = va_arg(args, uint32_t);
        fb_info.bg = va_arg(args, uint32_t);
    }

    uint32_t radix = 0;
    if (flags & NUM)
    {
        radix = va_arg(args, uint32_t);
    }

    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] == '%' && flags & NUM)
        {
            print_number(va_arg(args, uint64_t), radix);
            continue;
        }
        putc(str[i]);
    }

    if (!(flags & NOBREAK))
    {
        putc('\n');
    }

    fb_info.fg = old_fg;
    fb_info.bg = old_bg;

    va_end(args);
}
