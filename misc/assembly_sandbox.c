#include "../print_x86_64.h"
#include <stdio.h>

void gen_write_to_local_8byte(int offset);

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
	gen_push_from_local_nbyte(8, -8);
	gen_write_to_local_8byte(-16); // p = q;
	gen_discard();
	
	gen_push_int(171);
	gen_pop_to_reg_4byte("edi");

	gen_push_from_local_nbyte(8, -16);
	gen_pop_to_reg_8byte("r11");

	gen_call_reg_and_push_ret_of_nbyte(4, "r11");
	gen_epilogue_nbyte(4, 1);

	/*
int f(int a){
	return 3 + a;
}
	*/
	gen_prologue(0, "f");
	gen_write_register_to_local_4byte("edi", -4);
	gen_push_from_local_nbyte(4, -4);
	gen_push_int(3);
	gen_op_ints("addl");
	gen_epilogue_nbyte(4, 2);

	/*
void *b(void)
{
	return f;
}
	*/
	gen_prologue(0, "b");
	gen_push_address_of_global("f");
	gen_epilogue_nbyte(8, 3);
	
	/*
int main()
{
	return a(b());
}
	*/
	gen_prologue(16, "main");
	gen_push_ret_of_nbyte(8, "b");
	gen_pop_to_reg_8byte("rdi");
	gen_push_ret_of_nbyte(8, "a");
	gen_epilogue_nbyte(8, 4);
	
	return 0;
}
