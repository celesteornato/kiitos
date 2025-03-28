#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#define ISR_STUB(n) __attribute__((interrupt)) void isr_stub##n(void *)

ISR_STUB(0);
ISR_STUB(1);
ISR_STUB(2);
ISR_STUB(3);
ISR_STUB(4);
ISR_STUB(5);
ISR_STUB(6);
ISR_STUB(7);
ISR_STUB(8);
ISR_STUB(9);
ISR_STUB(10);
ISR_STUB(11);
ISR_STUB(12);
ISR_STUB(13);
ISR_STUB(14);
ISR_STUB(15);
ISR_STUB(16);
ISR_STUB(17);
ISR_STUB(18);
ISR_STUB(19);
ISR_STUB(20);
ISR_STUB(21);
ISR_STUB(22);
ISR_STUB(23);
ISR_STUB(24);
ISR_STUB(25);
ISR_STUB(26);
ISR_STUB(27);
ISR_STUB(28);
ISR_STUB(29);
ISR_STUB(30);
ISR_STUB(31);
__attribute__((interrupt)) void isr_kbinp(__attribute__((unused)) void *args);

#endif // INTERRUPTS_H_
