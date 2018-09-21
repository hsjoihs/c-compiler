#include "../header.h"
#include <stdio.h>
#include <stdlib.h>

struct UntypedExpr parse_assignment_expression(const struct Token **ptr_tokvec)
{
	exit(EXIT_FAILURE);
}

void check_declaration(const char *str)
{
	fprintf(stderr, "Parsing: %s\n", str);
	const struct Token *tokvec = read_and_preprocess(str);

	++tokvec; /* skip the dummy token BEGINNING */

	const char *ident_str;
	struct Type type = parse_struct_declaration(&tokvec, &ident_str);

	fprintf(stderr, "%s: ", ident_str);
	debug_print_type(&type);
	fprintf(stderr, "\n\n");
	return;
}

void check_typename(const char *str)
{
	fprintf(stderr, "Parsing: %s\n", str);
	const struct Token *tokvec = read_and_preprocess(str);

	++tokvec; /* skip the dummy token BEGINNING */

	struct Type type = parse_type_name(&tokvec);

	fprintf(stderr, "typename: ");
	debug_print_type(&type);
	fprintf(stderr, "\n\n");
	return;
}

int main()
{
	check_declaration("int **argv");
	check_declaration("int (*(*argv))");
	check_declaration("int arr[13]");
	check_declaration("int (*daytab)[13]");
	check_declaration("int (*(*x[3])[2])[4]");
	check_declaration("int (*(*x())[2])()");
	check_declaration("int *comp()");
	check_declaration("int (*comp)()");
	check_declaration("int (*(*x())[2])()");
	check_declaration("int atexit(int (*func)(int a))");
	check_declaration("int (*signal(int sig, int (*func)(int a)))(int b)");
	check_declaration("char **argv");
	check_declaration("char (*(*argv))");
	check_declaration("char arr[13]");
	check_declaration("char (*daytab)[13]");
	check_declaration("char (*(*x[3])[2])[4]");
	check_declaration("int atexit(int (*func)(int))");
	check_declaration("int (*signal(int, int (*func)(int)))(int)");

	check_typename("int **");
	check_typename("int [13]");
	check_typename("int (*)[13]");
	check_typename("int (*(*[3])[2])[4]");
	check_typename("int (*(*())[2])()");
	check_typename("int *()");
	check_typename("int (*)()");
	check_typename("int (*(*())[2])()");
	check_typename("int (int (*func)(int a))");
	check_typename("int (int (*func)(int))");
	check_typename("int (*(int sig, int (*func)(int a)))(int b)");
	check_typename("int (*(int, int (*func)(int)))(int)");
	return 0;
}
