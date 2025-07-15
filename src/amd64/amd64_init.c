#include "amd64/amd64_init.h"
#include "amd64/debug/logging.h"
#include "amd64/interrupts/idt.h"
#include "amd64/memory/gdt.h"
#include "amd64/memory/manager/vmm.h"
#include "fun/colors.h"

void arch_init(void)
{
    // Extra linebreak is wanted and cosmetic
    putsf("AMD64\n", COLOR, BLUE | GREEN, D_BLUE);

    __asm__ volatile("cli");

    puts("Setting up GDT...");
    gdt_init();
    puts("\tGDT set!");
    puts("Setting up IDT...");
    idt_init();
    puts("\tIDT set!");

    vmm_init();

    putc('\n');
}
