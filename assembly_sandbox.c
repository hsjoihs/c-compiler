#include "print_x86_64.h"
#include <stdio.h>

/*
char x[3];
char y[3];
char foo() { int c = 1; int d = 0; x[d] = y[c]; char e = 3; return e;}
int main(){ char f = foo(); return 174;}
*/

int main()
{
	gen_prologue(32, "foo");

	gen_push_int(1);
	gen_write_to_local(-4); /* c */
	gen_discard();

	gen_push_int(0);
	gen_write_to_local(-8); /* d */
	gen_discard();

	gen_push_from_local(-4); /* c */
	gen_cltq();
	gen_push_address_of_global("y");
	gen_op_8byte("addq");
	gen_pop_to_reg_8byte("rdx");
	puts("  movb (%rdx), %sil\n");

	gen_push_address_of_global("x");
	gen_push_from_local(-8);
	gen_cltq();
	gen_op_8byte("addq");
	gen_pop_to_reg_8byte("rdx");
	puts("	movb	%sil, (%rdx)\n");

	puts("	movb	$3, -9(%rbp)\n"
	     "  subq $8, %rsp\n"
	     "	movsbl	-9(%rbp), %eax\n"
	     "	movl	%eax, (%rsp)\n");
	gen_epilogue(134);

	/*



	*/
	gen_prologue(16, "main");
	gen_push_int(0);
	gen_write_to_local(-4);
	gen_discard();

	puts("	call	" PREFIX "foo\n"
	     "	movb	%al, -5(%rbp)\n");
	gen_push_int(174);
	gen_epilogue(123);
	gen_global_declaration("x", 3);
	gen_global_declaration("y", 3);
	return 0;
	/*
	gen_global_declaration(PREFIX "x", 3);
	gen_global_declaration(PREFIX "y", 3);
	puts("" PREFIX "foo:\n"
	     "  pushq %rbp\n"
	     "  movq %rsp, %rbp\n"
	     "  movl $1, -4(%rbp)\n"
	     "  movl $0, -8(%rbp)\n"
	     "  movl -4(%rbp), %eax\n"
	     "  cltq\n"
	     "  movzbl " PREFIX "y(%rax), %edx\n"
	     "  movl -8(%rbp), %eax\n"
	     "  cltq\n"
	     "  movb %dl, " PREFIX "x(%rax)\n"
	     "  movb $3, -9(%rbp)\n"
	     "  movzbl -9(%rbp), %eax\n"
	     "  popq %rbp\n"
	     "  ret\n");
	puts(".global " PREFIX "main\n"
	     "" PREFIX "main:\n"
	     "  pushq %rbp\n"
	     "  movq %rsp, %rbp\n"
	     "  subq $16, %rsp\n"
	     "  movl $0, %eax\n"
	     "  call " PREFIX "foo\n"
	     "  movb %al, -1(%rbp)\n"
	     "  movl $174, %eax\n"
	     "  leave\n"
	     "  ret\n");*/
}
