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
	const char* str = "123+456-789";
	get_token(&str);
	get_token(&str);
	get_token(&str);
	get_token(&str);
	printf("%s", str);
}

int foo(){
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
