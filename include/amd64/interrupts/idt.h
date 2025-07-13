#ifndef IDT_H_
#define IDT_H_

#include <stddef.h>
constexpr size_t IDT_LENGtH = 256;

void idt_init(void);

#endif // IDT_H_
