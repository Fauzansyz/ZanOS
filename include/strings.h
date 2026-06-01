#ifndef STRINGS_H
#define STRINGS_H

int strcmp(const char *a, const char *b);
void strcpy(char *dest, const char *src);
int strlen(const char *s);
void strncpy(char *dest, const char *src, int n);
int strncmp(const char *a, const char *b, int n);
int atoi(const char *s);
void *memcpy(void *dest, const void *src, int n);
void *memmove(void *dest, const void *src, int n);
int memcmp(const void *s1, const void *s2, int n);
void *memset(void *s, int c, int n);

#endif