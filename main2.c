#include "std.h"
#include "std_io.h"
#include "toplevel.h"

void main2(const struct Token *tokvec)
{
	const struct Vector /*<Toplevel>*/ vec = parse(tokvec);

	generate(&vec);
}