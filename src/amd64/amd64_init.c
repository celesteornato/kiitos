#include "amd64/amd64_init.h"
#include "amd64/framebuffer/logging.h"
#include "amd64/interrupts/idt.h"
#include "amd64/io/nvme/nvme_controls.h"
#include "amd64/memory/gdt.h"
#include "amd64/memory/manager/vmm.h"
#include "fun/colors.h"
#include <stddef.h>
#include <stdint.h>

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

    puts("Setting up new pagemap...");
    vmm_init();
    puts("\tPagemap set!");

    puts("Discovering NVMe...");
    uintptr_t nvme_baddr = 0;
    if (find_nvme_baddr(&nvme_baddr) != NVME_CONTROLS_OK)
    {
        putsf("Panic! Could not find nvme!", COLOR, RED, D_BLUE);
        __asm__("cli;hlt;");
    }
    putsf("\tFound NVMe at base address 0x%!", UNUM, 16, nvme_baddr);
}
