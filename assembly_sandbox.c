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
	puts(" movl $2, -4(%rbp)\n"
	     "movl $1, -8(%rbp)\n");

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

	puts("  movl -4(%rbp), %eax\n"
	     "  cltq\n"
	     "  leaq 0(,%rax,4), %rdx\n"
	     "  movq -16(%rbp), %rax\n"
	     "  addq %rdx, %rax\n"
	     "  movq %rax, -24(%rbp)\n"
	     "  movl -8(%rbp), %eax\n"
	     "  cltq\n"
	     "  leaq 0(,%rax,4), %rdx\n"
	     "  movq -24(%rbp), %rax\n"
	     "  addq %rdx, %rax\n"
	     "  movl (%rax), %edx\n"
	     "  movq -24(%rbp), %rax\n"
	     "  movl (%rax), %eax\n"
	     "  subl %eax, %edx\n"
	     "  movq -16(%rbp), %rax\n"
	     "  movl (%rax), %eax\n"
	     "  subl $2, %eax\n"
	     "  addl %eax, %eax\n"
	     "  addl %edx, %eax\n"
	     "  leave\n"
	     "  ret\n");
	return 0;
}
