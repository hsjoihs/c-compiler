#include "../print_x86_64.h"
#include "../print_x86_64_unofficial.h"
#include <stdio.h>

int main()
{

/*
struct A{ char a[n]; };

struct A derefn(struct A *p)
{
    return *p;
}

*/

	for (int n = 1; n <= 16; n++) {
		char str[8];
		sprintf(str, "deref%d", n);
		gen_prologue(8, str);
		gen_write_register_to_local_8byte("rdi", -8);
		gen_push_from_local_8byte(-8);
		gen_epilogue_returning_integerclass_struct_or_union(n, n+162);
	}

	return 0;
}
