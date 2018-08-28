#ifndef HEADER__STDH
#define HEADER__STDH

#ifndef OVERRIDE_STD
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define assert0 assert
#endif

#ifdef OVERRIDE_STD
void assert();
_Noreturn void assert0();
void *calloc();
void *realloc();
_Noreturn void exit();

#ifdef __STDC__
#define size_t2 unsigned long
#else
#define size_t2 int
#endif
size_t2 strlen();
int strcmp();
char *strcpy();
char *strcat();
char *strchr();

#define EXIT_FAILURE 1
#endif

#endif