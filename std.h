#ifndef HEADER__STDH
#define HEADER__STDH

#ifndef OVERRIDE_STD
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define assert0 assert
#endif

#ifdef OVERRIDE_STD

#ifdef __STDC__
#define size_t2 unsigned long
#endif
#ifndef __STDC__
#define size_t2 int
#endif

void assert();
_Noreturn void assert0(int i);
void *calloc(size_t2 nmemb, size_t2 size);
void *realloc(void *ptr, size_t2 size);
_Noreturn void exit(int status);

size_t2 strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *s1, const char *s2);
char *strcat(char *s1, const char *s2);
char *strchr(const char *s, int c);
int strncmp(const char *s1, const char *s2, size_t2 n);

#define EXIT_FAILURE 1
#endif

#endif
