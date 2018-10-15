#include "../print_x86_64.h"
#include <stdio.h>

int main()
{
	/*
int a(void* q){
	int (*p)(int) = q;
	return (*p)(171);
}

int f(int a){
	return 3 + a;
}

void *b(void)
{
	return f;
}

int main()
{
	return a(b());
}
	*/
	puts(
	    "	.globl	"PREFIX"a                      ## -- Begin function a\n"
	    ""PREFIX"a:                                     ## @a\n"
	    "	pushq	%rbp\n"
	    "	movq	%rsp, %rbp\n"
	    "	subq	$16, %rsp\n"
	    "	movl	$171, %eax\n"
	    "	movq	%rdi, -8(%rbp)\n"
	    "	movq	-8(%rbp), %rdi\n"
	    "	movq	%rdi, -16(%rbp)\n"
	    "	movl	%eax, %edi\n"
	    "	callq	*-16(%rbp)\n"
	    "	addq	$16, %rsp\n"
	    "	popq	%rbp\n"
	    "	retq\n");
	puts("	.globl	"PREFIX"f                      ## -- Begin function f\n"
	     ""PREFIX"f:                                     ## @f\n"
	     "	pushq	%rbp\n"
	     "	movq	%rsp, %rbp\n"
	     "	movl	%edi, -4(%rbp)\n"
	     "	movl	-4(%rbp), %edi\n"
	     "	addl	$3, %edi\n"
	     "	movl	%edi, %eax\n"
	     "	popq	%rbp\n"
	     "	retq\n");
	puts("	.globl	"PREFIX"b                      ## -- Begin function b\n"
	     ""PREFIX"b:                                     ## @b\n"
	     "	pushq	%rbp\n"
	     "	movq	%rsp, %rbp\n"
	     "	leaq	"PREFIX"f(%rip), %rax\n"
	     "	popq	%rbp\n"
	     "	retq\n");
	puts("	.globl	"PREFIX"main                   ## -- Begin function main\n"
	     ""PREFIX"main:                                  ## @main\n"
	     "	pushq	%rbp\n"
	     "	movq	%rsp, %rbp\n"
	     "	subq	$16, %rsp\n"
	     "	movl	$0, -4(%rbp)\n"
	     "	callq	"PREFIX"b\n"
	     "	movq	%rax, %rdi\n"
	     "	callq	"PREFIX"a\n"
	     "	addq	$16, %rsp\n"
	     "	popq	%rbp\n"
	     "	retq\n");
	return 0;
}
