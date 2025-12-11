#include "strings.h"

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

void strcpy(char *dest, const char *src) {
    while ((*dest++ = *src++));
}

int strlen(const char *s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void strncpy(char *dest, const char *src, int n) {
    int i;
    for (i = 0; i < n && src[i]; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
}

// implementasi mini strncmp
int strncmp(const char *a, const char *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i] || a[i] == '\0' || b[i] == '\0')
            return (unsigned char)a[i] - (unsigned char)b[i];
    }
    return 0;
}