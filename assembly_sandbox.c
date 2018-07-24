#include "header.h"
#include <stdio.h>

/*
int *alloc4(int a, int b, int c, int d);

int main(){
    int *p;
    p = alloc4(62, 8, 31, 85);
    int *q;
    q = p + 2;
    int *r;
    r = p + 3;
    return *r - *q + (*p-2)*2;
}

*/
int main()
{
	puts(".global _main\n"
	     "_main:\n"
	     "  pushq %rbp\n"
	     "  movq %rsp, %rbp\n"
	     "  subq $32, %rsp\n"
	     "  movl $85, %ecx\n"
	     "  movl $31, %edx\n"
	     "  movl $8, %esi\n"
	     "  movl $62, %edi\n"
	     "  call _alloc4\n"
	     "  movq %rax, -8(%rbp)\n"
	     "  movq -8(%rbp), %rax\n"
	     "  addq $8, %rax\n"
	     "  movq %rax, -16(%rbp)\n"
	     "  movq -8(%rbp), %rax\n"
	     "  addq $12, %rax\n"
	     "  movq %rax, -24(%rbp)\n"
	     "  movq -24(%rbp), %rax\n"
	     "  movl (%rax), %edx\n"
	     "  movq -16(%rbp), %rax\n"
	     "  movl (%rax), %eax\n"
	     "  subl %eax, %edx\n"
	     "  movq -8(%rbp), %rax\n"
	     "  movl (%rax), %eax\n"
	     "  subl $2, %eax\n"
	     "  addl %eax, %eax\n"
	     "  addl %edx, %eax\n"
	     "  leave\n"
	     "  ret\n");
	return 0;
}
