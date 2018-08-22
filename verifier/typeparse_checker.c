#include "../header.h"
#include <stdio.h>

void check(const char *str)
{
	fprintf(stderr, "Parsing: %s\n", str);
	const struct Token *tokvec = read_all_tokens(str);

	++tokvec; /* skip the dummy token BEGINNING */

	const char *ident_str;
	struct Type type = parse_declaration(&tokvec, &ident_str);

	fprintf(stderr, "%s: ", ident_str);
	debug_print_type(type);
	fprintf(stderr, "\n\n");
	return;
}

int main()
{
	check("int **argv");
	check("int (*(*argv))");
	check("int arr[13]");
	check("int (*daytab)[13]");
	check("int (*(*x[3])[2])[4]");
	check("int (*(*x())[2])()");
	check("int *comp()");
	check("int (*comp)()");
	check("int (*(*x())[2])()");
	check("int atexit(int (*func)(int a))");
	check("int (*signal(int sig, int (*func)(int a)))(int b)");
	check("char **argv");
	check("char (*(*argv))");
	check("char arr[13]");
	check("char (*daytab)[13]");
	check("char (*(*x[3])[2])[4]");
	return 0;
}
