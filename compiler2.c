#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "lexer.h"
#include "print_assembly.h"
#include "vector.h"

int get_precedence(enum TokenKind k)
{
	switch (k) {
		case OP_PLUS:
		case OP_MINUS:
			return -4;

		case OP_ASTERISK:
		case OP_SLASH:
		case OP_PERCENT:
			return -3;

		case LEFT_PAREN:
			return 123;

		case OP_COMMA:
			return -14;

		case OP_LT:
		case OP_GT:
		case OP_LT_EQ:
		case OP_GT_EQ:
			return -6;

		case OP_LSHIFT:
		case OP_RSHIFT:
			return -5;

		case OP_AND:
			return -8;

		case EMPTY:
		case RIGHT_PAREN:
		case END:
		case LIT_DEC_INTEGER:
			assert("NOT AN OPERATOR" && 0);
			break;
	}
	assert("unimplemented!!!!" && 0);
}

void print_op(struct Token tok)
{
	switch (tok.kind) {
		case OP_PLUS:
			op_ints("addl"); return;
		case OP_MINUS:
			op_ints("subl"); return;
		case OP_ASTERISK:
			mul_ints(); return;
		case OP_SLASH:
			div_ints(); return;
		case OP_PERCENT:
			rem_ints(); return;
		case OP_COMMA:
			op_ints("movl"); return;
		case OP_LT:
			compare_ints("setl"); return;
		case OP_LT_EQ:
			compare_ints("setle"); return;
		case OP_LSHIFT:
			shift_ints("sall"); return;
		case OP_GT:
			compare_ints("setg"); return;
		case OP_GT_EQ:
			compare_ints("setge"); return;
		case OP_RSHIFT:
			shift_ints("sarl"); return;
		case OP_AND:
			op_ints("andl"); return;

		case EMPTY:
		case LEFT_PAREN:
		case RIGHT_PAREN:
		case END:
		case LIT_DEC_INTEGER:
			assert("failure!!! not an op!!!!" && 0);
	}
	 
	assert("unimplemented!!!!" && 0);
	
}

void read_all_and_write_code(const char* str)
{
	struct vector_Token op_stack = init_vector_Token(0);

	print_header();

	struct Token tok;
	do {
		tok = get_token(&str);

		if (tok.kind == END) {
			break;
		}

		if (tok.kind == EMPTY) {
			continue;
		}

		if (tok.kind == LIT_DEC_INTEGER) {
			push_int(tok.int_value);
		} else if (tok.kind == LEFT_PAREN) {
			push_vector_Token(&op_stack, tok);
		} else if (tok.kind == RIGHT_PAREN) {
			while(op_stack.length > 0 && op_stack.vector[op_stack.length-1].kind != LEFT_PAREN){
				print_op(pop_vector_Token(&op_stack));
			}
			if (op_stack.length == 0){
				fprintf(stderr, "UNMATCHED BRACKET");
				abort();
			} else {
				pop_vector_Token(&op_stack);
			}
		} else { /* operators */
			while (op_stack.length > 0 && 
				(
					get_precedence(op_stack.vector[op_stack.length-1].kind) >
					get_precedence(tok.kind) 
					|| (
						get_precedence(op_stack.vector[op_stack.length-1].kind) ==
						get_precedence(tok.kind)
						&& 1 /* is left-associative */ 
					)
				) && op_stack.vector[op_stack.length-1].kind != LEFT_PAREN
				/* the operator at the top of the operator stack is not a left bracket */
			) {
				print_op(pop_vector_Token(&op_stack));
			}
			push_vector_Token(&op_stack, tok);
		}
	}while(1);

	while(op_stack.length > 0) {
		print_op(pop_vector_Token(&op_stack));
	}

	print_footer();
}

int main()
{
	char str[1000];
	/* const char* str = "123+456-789"; */
	scanf("%[^\n]s", str); /* VULNERABLE!!! */
	read_all_and_write_code(str);
}
