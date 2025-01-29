#include <stddef.h>
__attribute__((noreturn))
void halt_and_catch_fire(void);

__attribute__((noreturn))
void exception_handler(void);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

int kexec(char* cmd);
void kreboot(void);
