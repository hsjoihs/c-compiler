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
			return -3;
		case LEFT_PAREN:
			return 123;
		default:
			assert("NOT AN OPERATOR" && 0);
			break;
	}
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

		if (tok.kind == LIT_DEC_INTEGER) {
			push_int(tok.int_value);
		} else if (tok.kind == LEFT_PAREN) {
			push_vector_Token(&op_stack, tok);
		} else if (tok.kind == RIGHT_PAREN) {
			while(op_stack.length > 0 && op_stack.vector[op_stack.length-1].kind != LEFT_PAREN){
				struct Token last_tok = pop_vector_Token(&op_stack);
				if (last_tok.kind == OP_PLUS) {
					op_ints("addl");
				} else if (last_tok.kind == OP_MINUS) {
					op_ints("subl");
				} else if (last_tok.kind == OP_ASTERISK){
					mul_ints();
				} else {
					assert("gfdagaws" && 0);
				}
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
				struct Token last_tok = op_stack.vector[op_stack.length-1];
				--op_stack.length;
				if (last_tok.kind == OP_PLUS) {
					op_ints("addl");
				} else if (last_tok.kind == OP_MINUS) {
					op_ints("subl");
				} else if (last_tok.kind == OP_ASTERISK){
					mul_ints();
				} else {
					assert("gfjaekd;sx" && 0);
				}
			}
			push_vector_Token(&op_stack, tok);
		}
	}while(1);

	while(op_stack.length > 0) {
		struct Token last_tok = pop_vector_Token(&op_stack);
		if (last_tok.kind == OP_PLUS) {
			op_ints("addl");
		} else if (last_tok.kind == OP_MINUS) {
			op_ints("subl");
		} else if (last_tok.kind == OP_ASTERISK){
			mul_ints();
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
