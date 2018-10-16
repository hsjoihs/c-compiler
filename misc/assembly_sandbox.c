#include "../print_x86_64.h"
#include <stdio.h>

int main()
{
	/*
int a(void* q){
	int (*p)(int) = q;
	return (*p)(171);
}
	*/
	gen_prologue(100, "a");
	gen_write_register_to_local_8byte("rdi", -8); // void *q
	gen_push_from_local_8byte(-8);
	gen_write_to_local_8byte(-16); // p = q;
	gen_discard();
	
	gen_push_int(171);
	gen_pop_to_reg_4byte("edi");
	gen_call_local_fp_and_push_ret_of_4byte(-16);
	gen_epilogue(1);

	/*
int f(int a){
	return 3 + a;
}
	*/
	gen_prologue(0, "f");
	gen_write_register_to_local_4byte("edi", -4);
	gen_push_from_local_4byte(-4);
	gen_push_int(3);
	gen_op_ints("addl");
	gen_epilogue(2);

	/*
void *b(void)
{
	return f;
}
	*/
	gen_prologue(0, "b");
	gen_push_address_of_global("f");
	gen_epilogue_8byte(3);
	
	/*
int main()
{
	return a(b());
}
	*/
	gen_prologue(16, "main");
	gen_push_ret_of_8byte("b");
	gen_pop_to_reg_8byte("rdi");
	gen_push_ret_of_8byte("a");
	gen_epilogue_8byte(4);
	
	return 0;
}
