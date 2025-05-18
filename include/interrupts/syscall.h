#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <stdint.h>

// Defined in the coresponding architecture's directory at syscall.S
extern uint64_t syscall(uint8_t);

#endif // SYSCALL_H_
