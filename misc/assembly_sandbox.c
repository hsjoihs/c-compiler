#include "../print_x86_64.h"
#include <stdio.h>

int main()
{
	/*
struct B {int a; int b;};
struct D {int a; int b; char c;};
struct F {int k; int *a; int *b; char *c; char *d; int *e;};

struct B foo()
{
	struct B s;
	s.a = 1;
	return s;
}

struct D bar()
{
	struct D s;
	s.a = 1;
	return s;
}


struct F baz()
{
	struct F s;
	s.k = 172;
	return s;
}


int main()
{
	int num = 0;
	struct B b;
	b = foo();
	num += b.a;

	struct D a;
	a = bar();
	num += a.a;

	struct F c;
	c = baz();
	num += c.k;

	return num;
}

	*/
	gen_prologue(8, "foo");
	puts("  movl $1, -8(%rbp)\n");
	gen_push_address_of_local(-8);
	gen_epilogue_returning_small_struct(8, 5623);

	{
		gen_prologue(20, "bar");

		gen_push_int(1);
		gen_write_to_local(-24);
		gen_discard();
		puts(

		    "  movq -24(%rbp), %rax\n"
		    "  movq %rax, -12(%rbp)\n"

		    "  movl -16(%rbp), %eax\n"
		    "  movl %eax, -4(%rbp)\n");

		gen_push_address_of_local(-12);
		gen_epilogue_returning_small_struct(12, 875);
	}

	{
		/* baz */
		gen_prologue(8, "baz");

		gen_write_register_to_local_8byte("rdi", -56);

		gen_push_int(172);
		gen_write_to_local(-48);
		gen_discard();

		gen_push_from_local_8byte(-56);
		gen_push_address_of_local(-48);
		gen_copy_struct_and_discard(48);

		gen_epilogue_8byte(5463);
	}

	gen_prologue(160, "main");
	gen_push_int(0);
	gen_write_to_local(-4);
	gen_discard();

	gen_call_and_assign_small_struct_to_local("foo", -12, 8);
	puts("  movl -12(%rbp), %eax\n"
	     "  addl %eax, -4(%rbp)\n"
	     "  addq $8, %rsp\n");

	gen_call_and_assign_small_struct_to_local("bar", -24, 12);
	
	puts(
	     "  movl -24(%rbp), %eax\n"
	     "  addl %eax, -4(%rbp)\n"
	     "  addq $8, %rsp\n"
	     "  leaq -80(%rbp), %rax\n"
	     "  movq %rax, %rdi\n"
	     "  movl $0, %eax\n"
	     "  call " PREFIX "baz\n"
	     "  movl -80(%rbp), %eax\n"
	     "  addl %eax, -4(%rbp)\n"
	     "  subq $8, %rsp\n"
	     "  movl -4(%rbp), %eax\n"
	     "  movl  %eax, (%rsp)\n");
	gen_epilogue(425);
	return 0;
}
