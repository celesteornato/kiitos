#ifndef INT_KEYBOARD_H_
#define INT_KEYBOARD_H_

#include <stdint.h>

/*
** Concatenated bytes of size 1-6 wherein the Least Significant Byte represents the first-recieved
* byte of the key scancode sequence
*
** As it is 6 bytes at most, it can safely be treated as an int64_t
*/
extern uint64_t last_keypress;

#endif // INT_KEYBOARD_H_
