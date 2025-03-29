#include "interrupts.h"
#include "../basic/kio.h"

char text[20] = "Homly Smitt";

#define PIC1_sendEOI()                                                         \
  __asm__ volatile("outb %b0, %w1" : : "a"(0x20), "Nd"(0x20) : "memory")
#define PIC2_sendEOI()                                                         \
  __asm__ volatile("outb %b0, %w1" : : "a"(0xA0), "Nd"(0x20) : "memory")

__attribute__((interrupt)) void isr_stub0(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub1(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub2(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub3(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub4(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub5(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub6(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub7(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub8(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub9(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub10(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub11(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub12(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub13(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub14(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub15(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub16(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub17(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub18(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub19(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub20(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub21(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub22(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub23(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub24(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub25(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub26(__attribute__((unused)) void *arg) {
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub27(__attribute__((unused)) void *arg) {
  text[0] = 'G';
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub28(__attribute__((unused)) void *arg) {
  text[0] = 'G';
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub29(__attribute__((unused)) void *arg) {
  text[0] = 'G';
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub30(__attribute__((unused)) void *arg) {
  text[0] = 'G';
  __asm__("hlt");
}
__attribute__((interrupt)) void isr_stub31(__attribute__((unused)) void *arg) {
  text[0] = 'G';
  __asm__("hlt");
}

__attribute__((interrupt)) void isr_kbinp(__attribute__((unused)) void *args) {
  volatile unsigned char sc;
  __asm__ volatile("inb %w1, %b0" : "=a"(sc) : "Nd"(0x60) : "memory");

  text[0] = 'G';
  PIC1_sendEOI();
}
