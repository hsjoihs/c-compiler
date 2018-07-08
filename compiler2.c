#include <stdio.h>
#include <assert.h>
#include "lexer.h"
#include "print_assembly.h"


void read_all_and_write_code(const char* str)
{
	struct Token op_stack[3]; /* hasn't implemented any higher-order operator; thus, it suffices */
	int stack_len = 0;

	print_header();

	struct Token tok;
	do{
		tok = get_token(&str);

		if(tok.kind == END) {
			break;
		}

		if(tok.kind == LIT_DEC_INTEGER) {
			push_int(tok.int_value);
		} else { /* operators */
			while(stack_len > 0) {
				stack_len--;
				if(op_stack[stack_len].kind == OP_PLUS) {
					op_ints("addl");
				} else if(op_stack[stack_len].kind == OP_MINUS) {
					op_ints("subl");
				} else {
					assert(("gfjaekd;sx",0));
				}
			}
			op_stack[stack_len] = tok;
			stack_len++; 
		}
	}while(1);

	while(stack_len > 0) {
		stack_len--;
		if(op_stack[stack_len].kind == OP_PLUS) {
			op_ints("addl");
		} else if(op_stack[stack_len].kind == OP_MINUS) {
			op_ints("subl");
		} else {
			assert(("gfjaekd;sx",0));
		}
	}

	print_footer();
}

int main()
{
	char str[1000];
	/* const char* str = "123+456-789"; */
	scanf("%s", str); /* VULNERABLE!!! */
	read_all_and_write_code(str);
}
