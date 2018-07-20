#include "header.h"
#include <stdio.h>

/*

int main()
{
    int x;
    x = 174;
    int *y;
    y = &x;
    return *y;
 }
*/
int main()
{
	gen_prologue(0, "main");
	puts("  movl $174, -12(%rbp)\n"
	     "  leaq -12(%rbp), %rax\n"
	     "  movq %rax, -8(%rbp)\n"
	     "  movq -8(%rbp), %rax\n"
	     "  movl (%rax), %eax\n"
	     "  popq %rbp\n"
	     "  ret\n");
	return 0;
}
