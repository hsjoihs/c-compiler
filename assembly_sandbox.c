#include "header.h"
#include <stdio.h>

/*
int *alloc4(int a, int b, int c, int d);

int main(){
    int *p;
    int c = 2;
    int d = 1;
    p = alloc4(62, 8, 31, 85);
    int *q;
    q = p + c;
    return *(q+d) - *q + (*p-2)*2;
}

*/
int main()
{
	gen_prologue(32, "main");
	puts("  movl $2, -4(%rbp)\n"
	     "  movl $1, -8(%rbp)\n");

	/* p = alloc4(62, 8, 31, 85); */
	{
		/* alloc4(62, 8, 31, 85) */
		gen_push_int(85);
		gen_pop_to_reg("ecx");
		gen_push_int(31);
		gen_pop_to_reg("edx");
		gen_push_int(8);
		gen_pop_to_reg("esi");
		gen_push_int(62);
		gen_pop_to_reg("edi");
		gen_push_ret_of_8byte("alloc4");

		/* p = ...*/
		gen_write_to_local_8byte(-16);

		/* ; */
		gen_discard();
	}

	{
		/* c */
		gen_push_from_local(-4);
		/* convert long to quad */
		gen_cltq();

		/* c * 4 */
		gen_mul_by_const(4);

		/* p */
		gen_push_from_local_8byte(-16);

		gen_op_8byte("addq");

		/* q = ... */
		gen_write_to_local_8byte(-24);

		/* ; */
		gen_discard();
	}

	/* d */
	gen_push_from_local(-8);
	/* convert long to quad */
	gen_cltq();

	puts("  movq (%rsp), %rax\n"
	     "  leaq 0(,%rax,4), %rdx\n");
	gen_push_from_local_8byte(-24);
	puts("  movq (%rsp), %rax\n"
	     "  addq %rdx, %rax\n"
	     "  addq $8, %rsp\n"
	     "  movl (%rax), %edx\n"
	     "  addq $8, %rsp\n"
	     "  movq -24(%rbp), %rax\n"
	     "  movl (%rax), %eax\n"
	     "  subl %eax, %edx\n"
	     "  movq -16(%rbp), %rax\n"
	     "  movl (%rax), %eax\n"
	     "  subl $2, %eax\n"
	     "  subq $8, %rsp\n"
	     "  addl %eax, %eax\n"
	     "  movl %eax, (%rsp)\n"
	     "  addl %edx, (%rsp)\n");
	gen_epilogue_8byte(12345);
	return 0;
}
