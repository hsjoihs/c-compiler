#include "std.h"
#include "std_io.h"

void debug_write(const char *fmt, ...)
{
	struct va_list_tag ap[1];

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}
