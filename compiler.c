#include <stdio.h>
#include "lexer.h"

#define MACOS

#ifdef MACOS
#define PREFIX "_"
#else
#define PREFIX ""
#endif

int main()
{
	int num;
	scanf("%d", &num);
	printf(
		".global " PREFIX "main\n"
		PREFIX "main:\n" 
		"  movl $%d, %%eax\n"
		"  ret\n"
	, num);
	return 0;
}
