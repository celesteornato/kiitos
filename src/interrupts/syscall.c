#include "basic/fbio.h"
#include <io/keyboard.h>
#include <stdint.h>

/* The return type here is... debatable.
 * It is a uint64_t because syscall_internal's only goal is to put stuff into %rax.
 * no function will actually read syscall_internal's return value, and the assembly code that
 * will directly make syscalls will expect getting raw bytes back anyways.
 *
 * a.k.a B moment.
 */
uint64_t internal_syscall(uint8_t call_code)
{
    switch (call_code)
    {
    case 0:
        return (uint64_t)get_kb_char();
    default:
        return 0;
    }
}
