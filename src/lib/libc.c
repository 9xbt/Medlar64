#include <lib/libc.h>

int strlen(const char* pStr) {
    int i = 0;
    while (*pStr != '\0') {
        i++;
        pStr++;
    }
    return i;
}

int strcmp(const char* x, const char* y) {
    if (strlen(x) != strlen(y)) return 1;
    for (int i = 0; i < strlen(x); i++) {
        if (x[i] != y[i]) return 1;
    }
    return 0;
}

int strncmp(const char *x, const char *y, register size_t n)
{
    unsigned char u1, u2;

    while (n-- > 0)
    {
        u1 = (unsigned char) *x++;
        u2 = (unsigned char) *y++;
        if (u1 != u2)
	        return u1 - u2;
        if (u1 == '\0')
	        return 0;
    }
    return 0;
}

char* strcpy(char* dest, const char* src)
{
    if (dest == NULL) {
        return NULL;
    }
 
    char *ptr = dest;
 
    while (*src != '\0')
    {
        *dest = *src;
        dest++;
        src++;
    }
 
    *dest = '\0';
 
    return ptr;
}

char* strtok(char* str, const char* delim) {
    static char* p = NULL;
    if (str != NULL) { p = str; }
    else if (p == NULL) { return NULL; }

    char* start = p;
    while (*p != '\0')  {
        const char* d = delim;
        while (*d != '\0') {
        if (*p == *d) {
            *p = '\0';
            p++;
            if (start == p) {
            start = p;
            continue;
            }
            return start;
        }
        d++;
        }
        p++;
    }
    if (start == p) { return NULL; }
    return start;
}

void itoa(char *buf, int base, int d) {
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;

    if (base == 'd' && d < 0) {
        *p++ = '-';
        buf++;
        ud = -d;
    } else if (base == 'x')
        divisor = 16;

    do {
        int remainder = ud % divisor;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    } while (ud /= divisor);

    *p = 0;

    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

void *memcpy(void *dest, const void *src, size_t n) {
    asm volatile(
        "movq %0, %%rsi\n"
        "movq %1, %%rdi\n"
        "movq %2, %%rcx\n"
        "rep movsb\n"
        :
        : "r"(src), "r"(dest), "r"(n)
        : "%rsi", "%rdi", "%rcx", "memory"
    );
    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}