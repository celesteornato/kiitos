#include <stdint.h>
#include <stddef.h>
__attribute__((noreturn))
void halt_and_catch_fire(void);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

int kexec(char* cmd);
void kreboot(void);
