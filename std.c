#include "std.h"
#ifndef __S_TDC__
void assert(int i)
{
	if (!i) {
		exit(EXIT_FAILURE);
	}
}

_Noreturn void assert0(int i) { exit(EXIT_FAILURE + i); }
#endif
