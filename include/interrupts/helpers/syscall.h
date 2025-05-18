#ifndef TOOL_SYSCALL_H_
#define TOOL_SYSCALL_H_

#include <interrupts/syscall_values.h>
#include <stdint.h>

/*
** Defined in the coresponding architecture's directory at syscall.S
** Meant for internal use, so as to simplify making syscalls for debugging.
*/
uint64_t syscall_dbg(uint8_t call_code, uint64_t opt1, uint64_t opt2, uint64_t opt3, uint64_t opt4);

#endif // TOOL_SYSCALL_H_
