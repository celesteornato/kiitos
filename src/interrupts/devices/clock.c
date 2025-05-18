#include <interrupts/devices/clock.h>
#include <interrupts/pic.h>
#include <stdint.h>

_Atomic int64_t clock = 0;

void internal_clock(void)
{
    ++clock;
    pic_send_eoi(0);
}
