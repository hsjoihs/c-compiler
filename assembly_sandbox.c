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
	puts(".comm _y,8\n"
	     ".comm _x,4\n"
	     ".global _main\n"
	     "_main:\n"
	     "  pushq %rbp\n"
	     "  movq %rsp, %rbp\n"
	     "  movl $174, %ecx\n"
	     "  movl $3, %edx\n"
	     "  movl $2, %esi\n"
	     "  movl $1, %edi\n"
	     "  call _alloc4\n"
	     "  movq %rax, _y(%rip)\n"
	     "  movq _y(%rip), %rax\n"
	     "  addq $12, %rax\n"
	     "  movl (%rax), %edx\n"
	     "  movl _x(%rip), %eax\n"
	     "  addl %edx, %eax\n"
	     "  popq %rbp\n"
	     "  ret\n");
	return 0;
}
