#ifndef GPIODEFS_H_
#define GPIODEFS_H_

#include <stdint.h>
enum gpio_port : uint16_t {
    PS2_DATA = 0x60,
    PS2_COMMAND = 0x64,
    PIC1_COMMAND = 0x20,
    PIC2_COMMAND = 0xA0,
    PIC1_DATA = 0x21,
    PIC2_DATA = 0xA1
};

enum pic_message : uint8_t {
    ICW1_ICW4 = 0x01,      /* Indicates that ICW4 will be present */
    ICW1_SINGLE = 0x02,    /* Single (cascade) mode */
    ICW1_INTERVAL4 = 0x04, /* Call address interval 4 (8) */
    ICW1_LEVEL = 0x08,     /* Level triggered (edge) mode */
    ICW1_INIT = 0x10,      /* Initialization - required! */

    ICW4_8086 = 0x01,       /* 8086/88 (MCS-80/85) mode */
    ICW4_AUTO = 0x02,       /* Auto (normal) EOI */
    ICW4_BUF_SLAVE = 0x08,  /* Buffered mode/slave */
    ICW4_BUF_MASTER = 0x0C, /* Buffered mode/master */
    ICW4_SFNM = 0x10,       /* Special fully nested (not) */

    PIC_EOI = 0x20
};

enum ps2_message : uint8_t {
    PS2_SECONDARY_ENABLE1 = 0xA8,
    PS2_SECONDARY_ENABLE2 = 0x20,

    PS2_MOUSE_RESET = 0xFF,
    PS2_MOUSE_STREAM = 0xEA,
    PS2_MOUSE_DATA_REPORT = 0xF4,

    PS2_ACKNOWLEDGEMENT = 0xFA,
};

#endif // GPIODEFS_H_
