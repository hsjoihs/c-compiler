#ifndef HEADER__STDIOH
#define HEADER__STDIOH

#ifdef OSX
#define stderr __stderrp
#define stdin __stdinp
extern struct __FILE *stderr;
extern struct __FILE *stdin;
#endif
#ifndef OSX
extern struct __FILE *stderr;
extern struct __FILE *stdin;
#endif

#ifdef __STDC__
typedef struct __FILE FILE;
int fprintf(FILE *restrict, const char *restrict, ...);
int printf(const char *restrict, ...);
int sprintf(char *restrict s, const char *restrict format, ...);
#endif
#ifndef __STDC__
int fprintf();
int printf();
int sprintf();
#endif

char *fgets(char *buf, int a, struct __FILE *fp);
int puts(const char *str);
struct __FILE *fopen(const char *filename, const char *mode);

#endif
