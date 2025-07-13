#include "amd64/interrupts/idt.h"
#include <amd64/amd64_init.h>
#include <amd64/debug/logging.h>
#include <amd64/memory/gdt.h>
#include <fun/colors.h>

void arch_init(void)
{
    logsf("AMD64", COLOR, BLUE | GREEN, D_BLUE);

    __asm__ volatile("cli");

    logs("\nSetting up GDT...");
    gdt_init();
    logs("\tGDT set!");
    logs("\nSetting up IDT...");
    idt_init();
    logs("\tIDT set!");
    __asm__ volatile("sti");

    volatile int a = 1;
    volatile int b = 0;
    a /= b;

    logc('\n');
}
