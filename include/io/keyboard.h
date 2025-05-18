#ifndef IO_KEYBOARD_H_
#define IO_KEYBOARD_H_

enum KB_LAYOUT { US = 0, FR = 1 };

/*
** Gets the first character pressed AFTER the function is called
** Is that the POSIX way ? Not really. But you gotta admit the POSIX way to getchar is annyoing.
** Returns a char instead of an int because we're don't cotton around null-terminating streams
around these parts.
*/
char get_kb_char(void);

#endif // IO_KEYBOARD_H_
