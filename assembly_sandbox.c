#include "header.h"
#include <stdio.h>

/*
int x;
int *y;

int *alloc4(int a, int b, int c, int d);
int main() { y = alloc4(1,2,3,174); return x + *(y+3); }

*/
int main()
{
	puts(".comm _y,8\n");
	puts(".comm _x,4\n");
	gen_prologue(0, "main");

	gen_push_int(174);
	gen_pop_to_reg("ecx");

	gen_push_int(3);
	gen_pop_to_reg("edx");

	gen_push_int(2);
	gen_pop_to_reg("esi");

	gen_push_int(1);
	gen_pop_to_reg("edi");

	gen_push_ret_of_8byte("alloc4");

	gen_write_to_global_8byte("y");

	gen_discard();

	puts("  subq $8, %rsp\n"
	     "  movq _y(%rip), %rax\n"
	     "  movq %rax, (%rsp)\n"
	     "  movq (%rsp), %rax\n"
	     "  addq $8, %rsp\n"
	     "  addq $12, %rax\n"
	     "  movl (%rax), %edx\n"
	     "  movl _x(%rip), %eax\n"
	     "  addl %edx, %eax\n"
	     "  subq $8, %rsp\n"
	     "  movl %eax, (%rsp)\n");
	gen_epilogue(1234);
	return 0;
}
