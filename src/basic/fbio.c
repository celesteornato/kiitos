#include <basic/fbio.h>
#include <misc/colours.h>
#include <stdint.h>

// Can be aligned but I don't know why and that scares me a bit
struct PSF_font {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
};

volatile struct framebuffer_info_internal {
    size_t ppr;

    volatile uint32_t *fb;

    uint64_t width;
    uint64_t height;
    _Atomic uint64_t x; // Atomic because an interrupt might trigger while x/y are being updated
    _Atomic uint64_t y;
} __attribute__((aligned(64)));

extern const struct PSF_font
    _binary_src_assets_terminus_psf_start; // NOLINT as this is its object file name and it cannot
                                           // be changed easily
const struct PSF_font *default_font = &_binary_src_assets_terminus_psf_start;

static struct framebuffer_info_internal fb_info;

uint32_t fb_bg_color = BG;
uint32_t fb_fg_color = FG;

static inline void check_fb_bounds(void)
{
    const uint64_t max_col = fb_info.width / default_font->width;
    const uint64_t max_lines = fb_info.height / default_font->height;

    if (fb_info.x >= max_col)
    {
        fb_info.x = 0;
        fb_info.y = (fb_info.y + 1) % max_lines;
    }
    else if (fb_info.x < 0)
    {
        fb_info.x = max_col - 1;
        fb_info.y = (fb_info.y - 1) % max_lines;
    }
}

void k_internal_putc(char c, volatile uint32_t *fb, size_t ppr, uint64_t x, uint64_t y)
{

    const uint8_t *glyph_addr = (const uint8_t *)default_font + default_font->headersize +
                                ((uint64_t)c * default_font->bytesperglyph);

    uint64_t offset = (y * default_font->height * ppr) + (x * default_font->width);

    uint32_t bytesperline = (default_font->width + 7) / 8;
    for (y = 0; y < default_font->height; ++y)
    {
        uint64_t line = offset;
        for (x = 0; x < default_font->width; ++x)
        {
            const uint8_t current_bit = 0x80U >> (x & 7U);
            fb[line++] = (glyph_addr[x / 8] & current_bit) ? fb_fg_color : fb_bg_color;
        }
        glyph_addr += bytesperline;
        offset += ppr;
    }
}

void k_internal_puts(const char *s, volatile uint32_t *fb, size_t ppr, uint64_t x, uint64_t y,
                     uint64_t off_x, uint64_t off_y, uint64_t fbwidth, uint64_t fbheight)
{
    const uint64_t max_col = fbwidth / default_font->width;
    const uint64_t max_lines = fbheight / default_font->height;
    for (; *s != '\0'; ++s, ++x)
    {
        uint64_t tmp_x = off_x + (x % max_col);
        uint64_t tmp_y = off_y + ((y + (x / max_col)) % max_lines);
        switch (*s)
        {
        case '\n':
            ++y;
            x = 0;
            break;
        case '\t':
            x += 4;
            break;
        default:
            k_internal_putc(*s, fb, ppr, tmp_x, tmp_y);
        }
    }
}

void k_set_buff_settings(volatile uint32_t *fb, size_t ppr, uint64_t fbwidth, uint64_t fbheight)
{
    fb_info = (struct framebuffer_info_internal){
        .fb = fb, .ppr = ppr, .x = 0, .width = fbwidth, .height = fbheight};
}

void k_print(const char *str, uint32_t flags)
{
    // Two first bytes determine level of urgency:
    // None -> Warning -> Error -> Panic
    if (flags & URG1)
    {
        fb_fg_color = flags & URG2 ? FGKERNFAULT : FGERR;
    }
    else
    {
        fb_fg_color = flags & URG2 ? FGWR : FG;
    }

    const uint64_t max_lines = fb_info.height / default_font->height;
    for (uint64_t i = 0; str[i] != '\0'; ++i, ++fb_info.x)
    {
        check_fb_bounds();

        switch (str[i])
        {
        case '\n':
            fb_info.y = (fb_info.y + 1) % max_lines;
            fb_info.x = 0;
            break;
        case '\t':
            fb_info.x += 4;
            break;
        default:
            k_internal_putc(str[i], fb_info.fb, fb_info.ppr, fb_info.x, fb_info.y);
        }
    }
}
void k_puts(const char *str)
{
    k_print(str, 0);

    const uint64_t max_lines = fb_info.height / default_font->height;
    fb_info.y = (fb_info.y + 1) % max_lines;
    fb_info.x = 0;
}

void k_putc(char c)
{
    check_fb_bounds();
    switch (c)
    {
    case '\n':
        ++fb_info.y;
        fb_info.x = 0;
        break;
    case '\t':
        fb_info.x += 4;
        break;
    case '\b':
        if (fb_info.x > 0)
        {
            --fb_info.x;
        }
        k_internal_putc(' ', fb_info.fb, fb_info.ppr, fb_info.x, fb_info.y);
        break;
    default:
        k_internal_putc(c, fb_info.fb, fb_info.ppr, fb_info.x++, fb_info.y);
    }
}

void k_printd(int64_t n)
{
    check_fb_bounds();
    if (n == 0)
    {
        k_internal_putc('0', fb_info.fb, fb_info.ppr, fb_info.x++, fb_info.y);
        return;
    }
    if (n < 0)
    {
        k_internal_putc('-', fb_info.fb, fb_info.ppr, fb_info.x++, fb_info.y);
        n = -n;
    }

    // A 64-bit integer has at most 19 digits, we put a bit more to be safe
    char to_print[24] = {0};
    int idx = 0;

    while (n > 0)
    {
        to_print[idx++] = (char)(n % 10) + '0';
        n /= 10;
    }
    --idx;

    while (idx >= 0)
    {
        check_fb_bounds();
        k_internal_putc(to_print[idx--], fb_info.fb, fb_info.ppr, fb_info.x++, fb_info.y);
    }
}

void k_printd_base(int64_t n, uint8_t base)
{
    check_fb_bounds();
    if (n == 0)
    {
        k_internal_putc('0', fb_info.fb, fb_info.ppr, fb_info.x++, fb_info.y);
        return;
    }
    if (n < 0)
    {
        k_internal_putc('-', fb_info.fb, fb_info.ppr, fb_info.x++, fb_info.y);
        n = -n;
    }

    char to_print[65] = {0};
    int idx = 0;

    while (n > 0)
    {
        char to_add = (n % base > 9) ? 'A' - 10 : '0';
        to_print[idx++] = (char)(n % base) + to_add;
        n /= base;
    }
    --idx;

    while (idx >= 0)
    {
        check_fb_bounds();
        k_internal_putc(to_print[idx--], fb_info.fb, fb_info.ppr, fb_info.x++, fb_info.y);
    }
}

void k_paint(uint32_t color)
{
    for (uint64_t y = 0; y < fb_info.height; ++y)
    {
        for (uint64_t x = 0; x < fb_info.width; ++x)
        {
            fb_info.fb[x + (y * fb_info.width)] = color;
        }
    }
}
