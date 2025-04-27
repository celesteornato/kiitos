#ifndef PIC_H_
#define PIC_H_

#include <stdint.h>

/*
** Initialises the programmable interrupt chip. This should not be called out of
*kmain()'s init sequence.
*/
void pic_init(void);
#endif // PIC_H_
