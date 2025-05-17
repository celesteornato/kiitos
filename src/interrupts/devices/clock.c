#include <interrupts/devices/clock.h>
#include <interrupts/pic.h>
#include <stdint.h>

int64_t clock;

void internal_clock(void) {
    ++clock;
    pic_send_eoi(0);
}
