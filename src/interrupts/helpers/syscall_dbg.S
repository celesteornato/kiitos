.globl syscall_dbg
syscall_dbg:
    int $0x80
    ret
