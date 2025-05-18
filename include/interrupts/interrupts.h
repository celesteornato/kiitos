#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

// Undefined at end of file
#define ISR_STUB(n) void isr_##n(void)

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
ISR_STUB(32);
ISR_STUB(33);
ISR_STUB(34);
ISR_STUB(35);
ISR_STUB(36);
ISR_STUB(37);
ISR_STUB(38);
ISR_STUB(39);
ISR_STUB(40);
ISR_STUB(41);
ISR_STUB(42);
ISR_STUB(43);

extern void isr_0(void);
extern void isr_clock(void);
extern void isr_kbinp(void);
extern void isr_mouse(void);
extern void isr_syscall(void);

#undef ISR_STUB

#endif // INTERRUPTS_H_
