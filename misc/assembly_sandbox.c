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
	gen_prologue(100, "a");
	gen_write_register_to_local_8byte("rdi", -8); // void *q
	gen_push_from_local_8byte(-8);
	gen_write_to_local_8byte(-16); // p = q;
	gen_discard();
	
	gen_push_int(171);
	gen_pop_to_reg_4byte("edi");
	puts(
	    "  call *-16(%rbp)\n"
	    "  subq $8, %rsp\n"
	    "  movq %rax, (%rsp) \n");
	gen_epilogue(1);

	gen_prologue(0, "f");
	puts(
	     "	movl	%edi, -4(%rbp)\n"
	     "	movl	-4(%rbp), %edi\n"
	     "	addl	$3, %edi\n"
	     "	movl	%edi, %eax\n"
	     "	popq	%rbp\n"
	     "	ret\n");

	gen_prologue(0, "b");
	gen_push_address_of_global("f");
	puts(
	     "  addq $8, %rsp\n"
	     "	popq	%rbp\n"
	     "	ret\n");

	gen_prologue(16, "main");
	puts(
	     "	movl	$0, -4(%rbp)\n"
	     "	callq	"PREFIX"b\n"
	     "	movq	%rax, %rdi\n"
	     "	callq	"PREFIX"a\n"
	     "	addq	$16, %rsp\n"
	     "	popq	%rbp\n"
	     "	ret\n");
	return 0;
}
