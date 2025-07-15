#ifndef BASEFUNCS_H_
#define BASEFUNCS_H_

/* Even if not used, the basefunc translation unit must be linked with the
 * executable, as some C compilers reserve the right to use these functions no
 * matter the flags. */

#include <stddef.h>
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
#endif // BASEFUNCS_H_
