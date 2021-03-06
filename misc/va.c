#include <stdio.h>
#include <stdarg.h>

void debug_write(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}
