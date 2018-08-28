#include "../print_x86_64.h"
#include <stdio.h>

int main()
{
	/*
struct B {int a; int b;};
struct D {int a; int b; char c;};
struct F {int *a; int *b; char *c; char *d; int *e;};

struct B foo()
{
	struct B s;
	s.a = 1;
	s.b = 2;
	return s;
}

struct D bar()
{
	struct D s;
	s.a = 1;
	s.b = 2;
	return s;
}


struct F baz()
{
	struct F s;
	s.a = 0;
	s.b = 0;
	return s;
}


int main()
{
	struct B b;
	b = foo();

	struct D a;
	a = bar();

	struct F c;
	c = baz();
}

	*/
	gen_prologue(0, "foo");
	puts("  movl $1, -8(%rbp)\n"
	     "  movq -8(%rbp), %rax\n"
	     "  popq %rbp\n"
	     "  ret\n");

	gen_prologue(0, "bar");
	puts("  movl $1, -24(%rbp)\n"
	     "  movq -24(%rbp), %rax\n"
	     "  movq %rax, -12(%rbp)\n"
	     "  movl -16(%rbp), %eax\n"
	     "  movl %eax, -4(%rbp)\n"
	     "  movq -12(%rbp), %rdx\n"
	     "  movl -4(%rbp), %eax\n"
	     "  movq %rdx, %rcx\n"
	     "  movl %eax, %edx\n"
	     "  movq %rcx, %rax\n"
	     "  popq %rbp\n"
	     "  ret\n");

	gen_prologue(0, "baz");
	puts("  movq %rdi, -56(%rbp)\n"
	     "  movl $172, -48(%rbp)\n"
	     "  movq -56(%rbp), %rax\n"
	     "  movq -48(%rbp), %rdx\n"
	     "  movq -40(%rbp), %rcx\n"
	     "  movq %rdx, (%rax)\n"
	     "  movq %rcx, 8(%rax)\n"
	     "  movq -32(%rbp), %rdx\n"
	     "  movq -24(%rbp), %rcx\n"
	     "  movq %rdx, 16(%rax)\n"
	     "  movq %rcx, 24(%rax)\n"
	     "  movq -16(%rbp), %rdx\n"
	     "  movq -8(%rbp), %rcx\n"
	     "  movq %rdx, 32(%rax)\n"
	     "  movq %rcx, 40(%rax)\n"
	     "  movq -56(%rbp), %rax\n"
	     "  popq %rbp\n"
	     "  ret\n");

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
	     "  movl -4(%rbp), %eax\n"
	     "  leave\n"
	     "  ret\n");
	return 0;
}
