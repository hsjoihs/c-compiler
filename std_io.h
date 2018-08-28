#ifndef HEADER__STDIOH
#define HEADER__STDIOH

#ifdef OSX
#define stderr __stderrp
#define stdin __stdinp
extern struct __FILE *stderr;
extern struct __FILE *stdin;
#else
extern struct __FILE *stderr;
extern struct __FILE *stdin;
#endif

#ifdef __STDC__
typedef struct __FILE FILE;
int fprintf(FILE *restrict, const char *restrict, ...);
int printf(const char *restrict, ...);
#else
int fprintf();
int printf();
#endif

char *fgets(char *buf, int a, struct __FILE *fp);
int puts(const char *str);

#endif