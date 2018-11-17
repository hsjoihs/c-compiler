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

struct va_list_tag {
	/*unsigned*/ int gp_offset;
	/*unsigned*/ int fp_offset;
	void *overflow_arg_area;
	void *reg_save_area;
};

#ifdef __STDC__
#include <stdarg.h>
typedef struct __FILE FILE;
int fprintf(FILE *restrict, const char *restrict, ...);
int printf(const char *restrict, ...);
int sprintf(char *restrict s, const char *restrict format, ...);
int vfprintf(FILE *stream, const char *format, va_list arg);
int vprintf(const char *format, va_list arg);
#endif
#ifndef __STDC__
int fprintf();
int printf();
int sprintf();
int vfprintf(struct __FILE *stream, const char *format,
             struct va_list_tag arg[1]);
int vprintf(const char *format, struct va_list_tag arg[1]);
#endif

char *fgets(char *buf, int a, struct __FILE *fp);
int puts(const char *str);
struct __FILE *fopen(const char *filename, const char *mode);

#endif
