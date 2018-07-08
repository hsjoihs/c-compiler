#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"

int main()
{
	int num;
	scanf("%d", &num);

	print_header();
	push_int(num);
	print_footer();
	return 0;
}
