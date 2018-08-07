#include "print_x86_64.h"
#include <stdio.h>

int main()
{
	/*
	#include <stdio.h>
	int main()
	{
	    printf("%d %s", 1, "a");
	    return 174;
	}
	*/
	gen_str(0, "a");
	gen_str(1, "%d %s");

	gen_prologue(16, "main");
	gen_push_address_of_str(1);
	gen_pop_to_reg_8byte("rdi");

	gen_push_int(1);
	gen_pop_to_reg_4byte("esi");
	gen_push_address_of_str(0);
	gen_pop_to_reg_8byte("rdx");

	gen_push_int(0);
	gen_write_to_local(-4);
	gen_discard();

	gen_push_ret_of_4byte("printf");
	gen_discard();

	gen_push_int(174);
	gen_epilogue(12345);
	return 0;
}
