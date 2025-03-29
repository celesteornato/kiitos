#include "kiitklib.h"
#include "../kernel/kiitkio.h"
#include "../libc/kiitstdio.h"
#include "../libc/string/string.h"

__attribute__((noreturn)) void halt_and_catch_fire(void)
{
    __asm__ volatile("cli; hlt");
    while (1)
        ;
}

__attribute__((noreturn)) void exception_handler(void)
{
    __asm__ volatile("cli; hlt"); // Completely hangs the computer
    while (1)
        ;
}

void reboot(void)
{
    uint8_t temp;

    __asm__ volatile("cli");

    do {
        temp = inb(0x64);
        if ((temp & 1) != 0)
            inb(0x60);
    } while ((temp & 2) != 0);

    outb(0x64, 0xFE); /* pulse CPU reset line */
    __asm__ volatile("hlt");
    while (1)
        ;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; ++i)
        pdest[i] = psrc[i];

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; ++i)
        p[i] = (uint8_t)c;

    return s;
}

void *memmove(void *dest, const void *src, size_t n)
{
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; ++i)
            pdest[i] = psrc[i];
    } else if (src < dest) {
        for (size_t i = n; i > 0; --i)
            pdest[i - 1] = psrc[i - 1];
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i])
            return p1[i] < p2[i] ? -1 : 1;
    }

    return 0;
}

int kexec(char *cmd)
{
    if (cmd[0] == 0)
        return 0;

    char *args[10] = {0};
    unsigned int argc = 0;

    char *save_ptr;
    char *token = strtok_r(cmd, " ", &save_ptr);
    if (token == NULL)
        args[0] = cmd;
    while (token != NULL) {
        args[argc++] = token;
        token = strtok_r(NULL, " ", &save_ptr);
    }

    if (memcmp(args[0], "setfont", 8 * sizeof(char)) == 0) {
        if (argc == 1)
            return 1;
        if (args[1][0] - '0' > (int)(sizeof(PSF_DICT)/sizeof((PSF_DICT[0]))))
            return 4;
        k_setfont(PSF_DICT[args[1][0]-'0']);
        return 1;
    }
    if (memcmp(args[0], "ls", 3 * sizeof(char)) == 0) {
        return 5;
    }
    if (memcmp(args[0], "clear", 6 * sizeof(char)) == 0) {
        return 6;
    }
    if (memcmp(args[0], "reboot", 7 * sizeof(char)) == 0) {
        reboot();
        return 7;
    }
    return 3;
}
