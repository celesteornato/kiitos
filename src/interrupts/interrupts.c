#include <basic/fbio.h>
#include <interrupts/idt.h>
#include <interrupts/interrupts.h>
#include <interrupts/pic.h>
#include <misc/art.h>

#include <arch/x86/asm_functions.h>

#include <stdint.h>

const char *const error_messages[] = {
    "Division error: div by 0 or division exceeds bounds",
    "Debug",
    "Non-Maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device not available",
    "Double Fault",
    "Coprocessor Segment overrun (how old is your cpu!?)",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "GPF :(",
    "Page Fault",
    " ",
    "x87 FP-exception",
    "Alignment Check",
    "Machine Check",
    "SIMD FP-exception",
    "Virtualization Exception",
    "Control Point Exception",
    " ",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Invalid Foomp!!!!",
    "Triple Fault :(((",
    "FPU Error Interrupt (wow man your cpu is ooollld)",
};

[[noreturn]] void exception_handler(uint8_t err_code)
{
    k_print("Exception:\n\t", 0x3);
    k_print(error_messages[err_code], 0x3);
    k_print("\nKernel Panic!\n", 0x3);
    k_print(ASCII_KIITKAT, 0x2);
    hcf();
}

/*
 * Called from assembly, at x86/isr_stubs.S.
 * int_number being under 32 is undefined as all exceptions should instead call
 * the exception handler.
 * It will probably crash if you call it for an exception. This is good.
 * */
void unhandled(uint8_t int_number)
{
    // int_number refers to the absolute index of the interrupt in the IDT, we
    // adjust it to be from the PIC's point of view. int_number >= 32 so it fits
    // in a uint8.
    const uint8_t adjusted_int_number = int_number - IDT_HW_DESCRIPTORS;

    // PIC Interrupt 7 and 15 cannot physically exist, if they are triggered
    // it is a "spurious interrupt" and should be ignored.
    if (int_number == 7 || int_number == 15)
    {
        return;
    }

    k_print("Unhandled interrupt:\n\tCode ", 0x2);
    k_printd(int_number);
    k_print("\n", 0x2);

    pic_send_eoi(adjusted_int_number);
}
