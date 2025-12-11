#ifndef STRINGS_H
#define STRINGS_H

int strcmp(const char *a, const char *b);
void strcpy(char *dest, const char *src);
int strlen(const char *s);
void strncpy(char *dest, const char *src, int n);

// tambahkan strncmp
int strncmp(const char *a, const char *b, int n);

#endif