#ifndef LOGGING_H_
#define LOGGING_H_

#include <stddef.h>
#include <stdint.h>

enum log_flags : uint32_t {
    LOG_NOBREAK = 1 << 0, // Removes the linebreak at the end of dbg_putsf
    LOG_COLOR = 1 << 1,   // Specifies custom dbg_putsf fg+bg colours
    LOG_NUM = 1 << 2,     // Prints one number per % in the string, incompatible with UNUM
    LOG_UNUM = 1 << 3,    // Prints one unsigned number per % in the string, incompatible with NUM
};

// You MUST call this function before any other one in this header!
void logging_config(volatile uint32_t *fb, size_t width, size_t height, size_t ppr);
void change_fb_colors(uint32_t fg, uint32_t bg);

void clear_fb(uint32_t);
void putc(char);
void puts(const char *);

uintptr_t get_fb_address(void);
/* Returns the size, in bytes, of the whole framebuffer area */
uintptr_t get_fb_size(void);

/*
 * Put String with Flags. Flags are given in dbgio_flags.
 * The variable arguments are evaluated in that order, and no extra argument is required without
 * flags:
 ** Color: uint32_t fg, uint32_t bg
 ** NUM/UNUM: uint32_t radix, uint64_t value for each '%' in string (/!\ UNUM and NUM cannot be
 *together)
 */
void putsf(const char *str, uint32_t flags, ...);

#endif // LOGGING_H_
