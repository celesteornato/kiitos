#ifndef COLORS_H_
#define COLORS_H_

#include <stdint.h>

enum color : uint32_t {
    COLOR_WHITE = 0xffffff,
    COLOR_BLACK = 0x0,
    COLOR_RED = 0xff0000,
    COLOR_GREEN = 0x00ff00,
    COLOR_BLUE = 0x0000ff,
    COLOR_PURPLE = 0xff00ff,

    COLOR_D_RED = 0x722424,
    COLOR_D_GREEN = 0x3a6625,
    COLOR_D_BLUE = 0x245472,
    COLOR_D_PURPLE = 0x9b4c81,
};

#endif // COLORS_H_
