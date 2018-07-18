#include "header.h"
#include <stdio.h>

/*
int foo(int a){return a;}

int main() {
    int a;
    int b=0;
    int c = 0;
    for(
        a = 0;
         foo(a < 10) + c;
          a++){
        b += a + 2;
    }
    return a+b; }
*/
int main()
{
	gen_prologue(0, "foo");
	gen_write_register_to_local("edi", -4);
	gen_push_from_local(-4);
	gen_epilogue(123);

	gen_prologue(16, "main");
	gen_push_int(0);
	gen_write_to_local(-8);
	gen_discard();
	gen_push_int(0);
	gen_write_to_local(-12);
	gen_discard();
	gen_push_int(0);

	/* a = 0 */
	gen_write_to_local(-4); /* a */
	gen_discard();

	puts("  jmp .L4\n"
	     ".L5:\n");

	/*  b += a + 2; */
	gen_push_from_local(-4); /* a */
	gen_push_int(2);
	gen_op_ints("addl");
	gen_push_from_local(-8); /* b */
	gen_op_ints("addl");
	gen_write_to_local(-8);
	gen_discard();

	/* a++ */
	puts("  addl $1, -4(%rbp)\n");

	puts(".L4:\n"
	     "  cmpl $9, -4(%rbp)\n"
	     "  setle %al\n"
	     "  movzbl %al, %eax\n"
	     "  movl %eax, %edi\n"
	     "  call _foo\n"
	     "  movl %eax, %edx\n"
	     "  movl -12(%rbp), %eax\n"
	     "  addl %edx, %eax\n"
	     "  testl %eax, %eax\n"
	     "  jne .L5\n"
	     "  movl -4(%rbp), %edx\n"
	     "  movl -8(%rbp), %eax\n"
	     "  addl %edx, %eax\n"
	     "  leave\n"
	     "  ret\n");

	return 0;
}
