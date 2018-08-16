#include "print_x86_64.h"
#include <stdio.h>

int main()
{
	/*
struct A {
	char a; // offset: 0
	int b; // offset: 4
};

int foo(struct A* p)
{
	return p->b;
}

int main()
{
	struct A a;
	a.b = 3;
	int b = a.b;
	int c = foo(&a);
	return 168+b+c;
}


	*/

	gen_prologue(0, "foo");
	gen_write_register_to_local_8byte("rdi", -8);
	gen_push_from_local_8byte(-8);
	gen_push_int(4);
	gen_cltq();
	gen_op_8byte("addq");
	gen_peek_and_dereference_4byte();
	gen_epilogue(1234);

	gen_prologue(16, "main");

	gen_push_address_of_local(-16);
	gen_push_int(4);
	gen_cltq();
	gen_op_8byte("addq");
	gen_push_int(3);
	gen_assign_4byte();
	gen_discard();

	gen_push_address_of_local(-16);
	gen_push_int(4);
	gen_cltq();
	gen_op_8byte("addq");
	gen_peek_and_dereference_4byte();
	gen_write_to_local(-4);
	gen_discard();

	gen_push_address_of_local(-16);
	gen_pop_to_reg_8byte("rdi");
	gen_push_ret_of_4byte("foo");
	gen_write_to_local(-8);
	gen_discard();

	puts(
	     "  movl -4(%rbp), %eax\n"
	     "  addl $168, %eax\n"
	     "  movl %eax, %edx\n"
	     "  movl -8(%rbp), %eax\n"
	     "  addl %edx, %eax\n"
	     "  leave\n"
	     "  ret\n");
	return 0;
}
