#include "print_x86_64.h"
#include <stdio.h>

int main()
{
	/*

struct A {int a; char c; char d; int b; char e; };

int main()
{

	struct A a;
	struct A b;
	a = b;
	return 174;
}

	*/
	gen_prologue(60, "main");
	gen_push_address_of_local(-16);
	gen_push_address_of_local(-32);

	gen_copy_struct_and_discard(16);

	gen_push_int(174);
	gen_epilogue(543);
	return 0;
}
