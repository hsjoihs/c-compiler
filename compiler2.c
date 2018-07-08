#include <stdio.h>
#include <assert.h>
#include "lexer.h"
#include "print_assembly.h"
#include "vector.h"


void read_all_and_write_code(const char* str)
{
	struct vector_Token op_stack = init_vector_Token(0);

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
			while(op_stack.length > 0) {
				--op_stack.length;
				if(op_stack.vector[op_stack.length].kind == OP_PLUS) {
					op_ints("addl");
				} else if(op_stack.vector[op_stack.length].kind == OP_MINUS) {
					op_ints("subl");
				} else {
					assert("gfjaekd;sx" && 0);
				}
			}
			push_vector_Token(&op_stack, tok);
		}
	}while(1);

	while(op_stack.length > 0) {
		--op_stack.length;
		if(op_stack.vector[op_stack.length].kind == OP_PLUS) {
			op_ints("addl");
		} else if(op_stack.vector[op_stack.length].kind == OP_MINUS) {
			op_ints("subl");
		} else {
			assert("gfdagaws" && 0);
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
