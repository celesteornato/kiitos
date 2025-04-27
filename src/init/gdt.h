#ifndef GDT_H_
#define GDT_H_

/*
** Initialises the global descriptor table, should not be used out of
*kmain()'s init sequence.
*/
void gdt_init(void);

#endif // GDT_H_
