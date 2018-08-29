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
	puts("  movl $1, -8(%rbp)\n"
	     "  subq $8, %rsp\n"
	     "  movq -8(%rbp), %rax\n"
	     "  movq %rax, (%rsp)\n");
	gen_epilogue_8byte(523);

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
		puts(
			 "  movq (%rsp), %rcx\n"
			 "  addq $8, %rsp\n"
			 "  movq (%rcx), %rax\n"
		     "  movl 8(%rcx), %edx\n"
		     "  leave\n"
		     "  ret\n"
		     );
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

	gen_prologue(80, "main");
	puts("  movl $0, -4(%rbp)\n"
	     "  movl $0, %eax\n"
	     "  call " PREFIX "foo\n"
	     "  movq %rax, -12(%rbp)\n"
	     "  movl -12(%rbp), %eax\n"
	     "  addl %eax, -4(%rbp)\n"
	     "  movl $0, %eax\n"
	     "  call " PREFIX "bar\n"
	     "  movq %rax, %rcx\n"
	     "  movl %edx, %eax\n"
	     "  movq %rcx, -24(%rbp)\n"
	     "  movl %eax, -16(%rbp)\n"
	     "  movl -24(%rbp), %eax\n"
	     "  addl %eax, -4(%rbp)\n"
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
