#ifndef COLOURS_H_
#define COLOURS_H_

#include <stdint.h>

enum COLOR : uint32_t {
    BG = 0x0e3d37,
    FG = 0xffffff,
    FGWR = 0xff44ff,
    FGERR = 0xff00ff,
    FGKERNFAULT = 0xff1111,
    FOOMP_COLOR = 0x0220FF
};

extern enum COLOR fb_bg_color;
extern enum COLOR fb_fg_color;

#endif // COLOURS_H_
