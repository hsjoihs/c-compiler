#include "header.h"
#include <stdio.h>

void foo(const char *str)
{
	fprintf(stderr, "Parsing: %s\n", str);
	const struct Token *tokvec = read_all_tokens_(str);

	++tokvec; /* skip the dummy token BEGINNING */

	const char *ident_str;
	struct Type type = parse_var_declarator(&tokvec, &ident_str);

	fprintf(stderr, "%s: ", ident_str);
	debug_print_type(type);
	fprintf(stderr, "\n\n");
	return;
}

int main()
{
	foo("int **argv");
	foo("int (*(*argv))");
	foo("int arr[13]");
	foo("int (*daytab)[13]");
	foo("int (*(*x[3])[2])[4]");
	foo("int (*(*x())[2])()");
	foo("int *comp()");
	foo("int (*comp)()");
	foo("int (*(*x())[2])()");
	return 0;
}
