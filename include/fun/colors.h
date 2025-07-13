#ifndef COLORS_H_
#define COLORS_H_

#include <stdint.h>

enum color : uint32_t {
    WHITE = 0xffffff,
    BLACK = 0x0,
    RED = 0xff0000,
    GREEN = 0x00ff00,
    BLUE = 0x0000ff,
    PURPLE = 0xff00ff,

    D_RED = 0x722424,
    D_GREEN = 0x3a6625,
    D_BLUE = 0x245472,
    D_PURPLE = 0x9b4c81,
};

#endif // COLORS_H_
