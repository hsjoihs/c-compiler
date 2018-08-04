#include "print_x86_64.h"
#include <stdio.h>

/*
int main(){int a[1][2];int (*p)[2];p = a;int *q;q = *(p+1); *q=174; return
**(a+1);}
*/

int main()
{
	gen_prologue(0, "main");
	puts("  leaq -24(%rbp), %rax\n"
	     "  movq %rax, -8(%rbp)\n"
	     "  movq -8(%rbp), %rax\n"
	     "  addq $8, %rax\n"
	     "  movq %rax, -16(%rbp)\n"
	     "  movq -16(%rbp), %rax\n"
	     "  movl $174, (%rax)\n"
	     "  movl -16(%rbp), %eax\n"
	     "  popq %rbp\n"
	     "  ret\n");
}
