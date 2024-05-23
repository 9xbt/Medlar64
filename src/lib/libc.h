#pragma once

#include <stdint.h>
#include <stddef.h>

int strlen(const char* pStr);
int strcmp(const char* x, const char* y);
int strncmp(const char* x, const char* y, size_t n);
char* strcpy(char* dest, const char* src);
char* strtok(char* str, const char* delim);
void itoa(char *buf, int base, int d);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);