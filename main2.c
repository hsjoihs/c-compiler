#include "std.h"
#include "std_io.h"
#include "toplevel.h"

void main2(const char *str)
{
	const struct Token *tokvec = read_all_tokens(str);

	++tokvec; /* skip the dummy token BEGINNING */

	const struct Vector /*<Toplevel>*/ vec = parse(tokvec);

	generate(&vec);
}