#include <stddef.h>

size_t strlen(const char *s);
size_t strcspn(const char *s, const char *c);
size_t strspn(const char *s, const char *c);
char *strchrnul(const char *s, int c);
char *strtok(char *__restrict __s, const char *__restrict __delim);
char *strtok_r(char *__restrict __s, const char *__restrict __delim,
                        char **__restrict __save_ptr);
