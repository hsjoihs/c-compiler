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

	gen_prologue(8, "bar");
	puts("  movl $1, -24(%rbp)\n"
	     "  movq -24(%rbp), %rax\n"
	     "  movq %rax, -12(%rbp)\n"
	     "  movl -16(%rbp), %eax\n"
	     "  movl %eax, -4(%rbp)\n"
	     "  movq -12(%rbp), %rdx\n"
	     "  movl -4(%rbp), %eax\n"
	     "  movq %rdx, %rcx\n"
	     "  movl %eax, %edx\n"
	     "  subq $8, %rsp\n"
	     "  movq %rcx, (%rsp)\n");
	gen_epilogue_8byte(5423);

	/* baz */
	gen_prologue(8, "baz");

	gen_write_register_to_local_8byte("rdi", -56);

	gen_push_int(172);
	gen_write_to_local(-48);
	gen_discard();
	puts(
	     "  movq -56(%rbp), %rdx\n"
	     "  movq -48(%rbp), %rax\n"
	     "  movq %rax, (%rdx)\n"
	     "  movq -40(%rbp), %rax\n"
	     "  movq %rax, 8(%rdx)\n"
	     "  movq -32(%rbp), %rax\n"
	     "  movq %rax, 16(%rdx)\n"
	     "  movq -24(%rbp), %rax\n"
	     "  movq %rax, 24(%rdx)\n"
	     "  movq -16(%rbp), %rax\n"
	     "  movq %rax, 32(%rdx)\n"
	     "  movq -8(%rbp), %rax\n"
	     "  movq %rax, 40(%rdx)\n"
	     "  subq $8, %rsp\n"
	     "  movq -56(%rbp), %rdx\n"
	     "  movq %rdx, (%rsp)\n");
	gen_epilogue_8byte(5463);

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
