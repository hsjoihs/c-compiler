#include "header.h"
#include "intmap.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ParserState {
	struct int_map var_table;
	int final_label_name;
	int return_label_name;
};

void error_unexpected_token(struct Token token, const char *str);
int get_label_name(struct ParserState *ptr_ps);

void print_unary_prefix_op(enum TokenKind kind)
{
	switch (kind) {
		case OP_NOT:
			gen_unary_not();
			return;
		case OP_TILDA:
			gen_unary("notl");
			return;
		case OP_PLUS:
			/* do nothing */
			return;
		case OP_MINUS:
			gen_unary("negl");
			return;
		case IDENT_OR_RESERVED:
		case LEFT_PAREN:
		case RIGHT_PAREN:
		case END:
		case LIT_DEC_INTEGER:
			assert("failure!!! not an op!!!!" && 0);
		default:
			assert("failure!!! not a unary prefix op!!!!" && 0);
	}
	assert("unimplemented!!!!" && 0);
}

void binary_op(enum TokenKind kind)
{
	switch (kind) {
		case OP_PLUS:
			gen_op_ints("addl");
			return;
		case OP_MINUS:
			gen_op_ints("subl");
			return;
		case OP_ASTERISK:
			gen_mul_ints();
			return;
		case OP_SLASH:
			gen_div_ints();
			return;
		case OP_PERCENT:
			gen_rem_ints();
			return;
		case OP_COMMA:
			gen_op_ints("movl");
			return;
		case OP_LT:
			gen_compare_ints("setl");
			return;
		case OP_LT_EQ:
			gen_compare_ints("setle");
			return;
		case OP_LSHIFT:
			gen_shift_ints("sall");
			return;
		case OP_GT:
			gen_compare_ints("setg");
			return;
		case OP_GT_EQ:
			gen_compare_ints("setge");
			return;
		case OP_RSHIFT:
			gen_shift_ints("sarl");
			return;
		case OP_AND:
			gen_op_ints("andl");
			return;
		case OP_OR:
			gen_op_ints("orl");
			return;
		case OP_EQ_EQ:
			gen_compare_ints("sete");
			return;
		case OP_NOT_EQ:
			gen_compare_ints("setne");
			return;
		case OP_HAT:
			gen_op_ints("xorl");
			return;

		case OP_EQ:
		case OP_PLUS_EQ:
		case OP_MINUS_EQ:
		case OP_ASTERISK_EQ:
		case OP_SLASH_EQ:
		case OP_PERCENT_EQ:
		case OP_LSHIFT_EQ:
		case OP_RSHIFT_EQ:
		case OP_AND_EQ:
		case OP_HAT_EQ:
		case OP_OR_EQ:
		case OP_AND_AND:
		case OP_OR_OR:
			assert("failure!!! must be handled separately!!!!" && 0);

		case LEFT_BRACE:
		case RIGHT_BRACE:
		case QUESTION:
		case COLON:
		case OP_NOT:
		case OP_TILDA:
		case IDENT_OR_RESERVED:
		case LEFT_PAREN:
		case RIGHT_PAREN:
		case END:
		case LIT_DEC_INTEGER:
		case RES_RETURN:
		case SEMICOLON:
		case RES_IF:
		case RES_ELSE:
		case RES_WHILE:
		case RES_DO:
			assert("failure!!! not a binary op!!!!" && 0);
	}

	assert("unimplemented!!!!" && 0);
}

void read_all_tokens_debug(const char *str)
{
	struct Token tok;
	do {
		tok = get_token(&str);
		print_token(tok);
		fprintf(stderr, "\n");
		if (tok.kind == END) {
			break;
		}
	} while (1);
}

void parse_expression(struct ParserState *ptr_ps,
                      const struct Token **ptr_to_tokvec);
void parse_assignment_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_to_tokvec);
void parse_additive_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_to_tokvec);
void parse_multiplicative_expression(struct ParserState *ptr_ps,
                                     const struct Token **ptr_to_tokvec);
void parse_primary_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_to_tokvec);
void parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_to_tokvec);
void parse_exclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_to_tokvec);
void parse_AND_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_to_tokvec);
void parse_AND_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_to_tokvec);
void parse_equality_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_to_tokvec);
void parse_relational_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_to_tokvec);
void parse_shift_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_to_tokvec);
void parse_cast_expression(struct ParserState *ptr_ps,
                           const struct Token **ptr_to_tokvec);
void parse_unary_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_to_tokvec);
void parse_postfix_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_to_tokvec);
void parse_conditional_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_to_tokvec);
void parse_compound_statement(struct ParserState *ptr_ps,
                              const struct Token **ptr_to_tokvec);
void parse_logical_OR_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_to_tokvec);
void parse_logical_AND_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_to_tokvec);

void parse_expression(struct ParserState *ptr_ps,
                      const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_assignment_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_COMMA) {
			break;
		}
		++tokvec;
		parse_assignment_expression(ptr_ps, &tokvec);
		binary_op(OP_COMMA);
	}
	*ptr_to_tokvec = tokvec;
}

int from_name(struct ParserState ps, const char *str)
{
	return lookup(ps.var_table, str);
}

void before_assign(enum TokenKind kind)
{
	switch (kind) {
		case OP_PLUS_EQ:
			binary_op(OP_PLUS);
			return;
		case OP_MINUS_EQ:
			binary_op(OP_MINUS);
			return;
		case OP_ASTERISK_EQ:
			binary_op(OP_ASTERISK);
			return;
		case OP_SLASH_EQ:
			binary_op(OP_SLASH);
			return;
		case OP_PERCENT_EQ:
			binary_op(OP_PERCENT);
			return;
		case OP_LSHIFT_EQ:
			binary_op(OP_LSHIFT);
			return;
		case OP_RSHIFT_EQ:
			binary_op(OP_RSHIFT);
			return;
		case OP_AND_EQ:
			binary_op(OP_AND);
			return;
		case OP_HAT_EQ:
			binary_op(OP_HAT);
			return;
		case OP_OR_EQ:
			binary_op(OP_OR);
			return;
		default:
			assert("cannot happen" && 0);
	}
}

void parse_assignment_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;

	enum TokenKind opkind = tokvec[1].kind;

	if (tokvec[0].kind == IDENT_OR_RESERVED &&
	    (opkind == OP_EQ || opkind == OP_PLUS_EQ || opkind == OP_MINUS_EQ ||
	     opkind == OP_ASTERISK_EQ || opkind == OP_SLASH_EQ ||
	     opkind == OP_PERCENT_EQ || opkind == OP_LSHIFT_EQ ||
	     opkind == OP_RSHIFT_EQ || opkind == OP_AND_EQ || opkind == OP_HAT_EQ ||
	     opkind == OP_OR_EQ)) {
		const char *name = tokvec[0].ident_str;
		tokvec += 2;
		*ptr_to_tokvec = tokvec;

		if (opkind != OP_EQ) {
			printf("//load from `%s`\n", name);
			gen_push_from_local(from_name(*ptr_ps, name));
		}

		parse_assignment_expression(ptr_ps, &tokvec);

		if (opkind != OP_EQ) {
			printf("//before assigning to `%s`:\n", name);
			before_assign(opkind);
		}

		printf("//assign to `%s`\n", name);
		gen_write_to_local(from_name(*ptr_ps, name));
	} else {
		parse_conditional_expression(ptr_ps, &tokvec);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_conditional_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_logical_OR_expression(ptr_ps, &tokvec);
	if (tokvec[0].kind == QUESTION) {
		int label1 = get_label_name(ptr_ps);
		int label2 = get_label_name(ptr_ps);

		gen_ternary_part1(label1, label2);
		++tokvec;
		*ptr_to_tokvec = tokvec;
		parse_expression(ptr_ps, &tokvec);

		gen_ternary_part2(label1, label2);

		if (tokvec[0].kind != COLON) {

			error_unexpected_token(tokvec[0],
			                       "colon of the conditional operator");
		}

		++tokvec;
		*ptr_to_tokvec = tokvec;
		parse_conditional_expression(ptr_ps, &tokvec);

		gen_ternary_part3(label1, label2);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_logical_OR_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	int label1 = get_label_name(ptr_ps);
	int label2 = get_label_name(ptr_ps);

	int counter = 0;
	parse_logical_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR_OR) {
			break;
		}

		if (counter == 0) {
			gen_logical_OR_set(0, label1, label2);
		}

		++tokvec;
		parse_logical_AND_expression(ptr_ps, &tokvec);
		++counter;
		gen_logical_OR_set(counter, label1, label2);
	}

	if (counter != 0) {
		gen_logical_OR_final(counter, label1, label2);
	}

	*ptr_to_tokvec = tokvec;
}

void parse_logical_AND_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	int label1 = get_label_name(ptr_ps);
	int label2 = get_label_name(ptr_ps);

	int counter = 0;
	parse_inclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND_AND) {
			break;
		}

		if (counter == 0) {
			gen_logical_AND_set(0, label1, label2);
		}

		++tokvec;
		parse_inclusive_OR_expression(ptr_ps, &tokvec);
		++counter;
		gen_logical_AND_set(counter, label1, label2);
	}

	if (counter != 0) {
		gen_logical_AND_final(counter, label1, label2);
	}

	*ptr_to_tokvec = tokvec;
}

void parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_exclusive_OR_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR) {
			break;
		}
		++tokvec;
		parse_exclusive_OR_expression(ptr_ps, &tokvec);
		binary_op(kind);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_exclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_AND_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_HAT) {
			break;
		}
		++tokvec;
		parse_AND_expression(ptr_ps, &tokvec);
		binary_op(kind);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_AND_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_equality_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND) {
			break;
		}
		++tokvec;
		parse_equality_expression(ptr_ps, &tokvec);
		binary_op(kind);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_equality_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_relational_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_EQ_EQ && kind != OP_NOT_EQ) {
			break;
		}
		++tokvec;
		parse_relational_expression(ptr_ps, &tokvec);
		binary_op(kind);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_relational_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_shift_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_GT && kind != OP_GT_EQ && kind != OP_LT &&
		    kind != OP_LT_EQ) {
			break;
		}
		++tokvec;
		parse_shift_expression(ptr_ps, &tokvec);
		binary_op(kind);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_shift_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_additive_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_LSHIFT && kind != OP_RSHIFT) {
			break;
		}
		++tokvec;
		parse_additive_expression(ptr_ps, &tokvec);
		binary_op(kind);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_additive_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_multiplicative_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_PLUS && kind != OP_MINUS) {
			break;
		}
		++tokvec;
		parse_multiplicative_expression(ptr_ps, &tokvec);
		binary_op(kind);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_multiplicative_expression(struct ParserState *ptr_ps,
                                     const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_cast_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_ASTERISK && kind != OP_SLASH && kind != OP_PERCENT) {
			break;
		}
		++tokvec;
		parse_cast_expression(ptr_ps, &tokvec);
		binary_op(kind);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_cast_expression(struct ParserState *ptr_ps,
                           const struct Token **ptr_to_tokvec)
{
	parse_unary_expression(ptr_ps, ptr_to_tokvec);
}

void parse_unary_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;

	/* unary-operator cast-expression */
	if (tokvec[0].kind == OP_NOT || tokvec[0].kind == OP_TILDA ||
	    tokvec[0].kind == OP_PLUS || tokvec[0].kind == OP_MINUS) {
		enum TokenKind kind = tokvec[0].kind;
		++tokvec;
		parse_cast_expression(ptr_ps, &tokvec);
		print_unary_prefix_op(kind);
	} else {
		parse_postfix_expression(ptr_ps, &tokvec);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_postfix_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == LEFT_PAREN) {
		const char *ident_str = tokvec[0].ident_str;
		if (tokvec[2].kind == RIGHT_PAREN) {
			gen_push_ret_of(ident_str);
			tokvec += 3;
		} else {
			const char *regs[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
			tokvec += 2;
			int counter = 0;

			parse_assignment_expression(ptr_ps, &tokvec);
			gen_pop_to_reg(regs[counter]);
			counter++;
			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;
				parse_assignment_expression(ptr_ps, &tokvec);
				if (counter > 5) {
					fprintf(
					    stderr,
					    "calling with 7 or more arguments is unimplemented!\n");
					abort();
				}
				gen_pop_to_reg(regs[counter]);
				counter++;
			}

			gen_push_ret_of(ident_str);
			*ptr_to_tokvec = tokvec;

			if (tokvec[0].kind != RIGHT_PAREN) {
				error_unexpected_token(tokvec[0],
				                       "closing parenthesis of function call");
			}
			++tokvec;
			*ptr_to_tokvec = tokvec;
		}

	} else {
		parse_primary_expression(ptr_ps, &tokvec);
	}
	*ptr_to_tokvec = tokvec;
}

void parse_primary_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == LIT_DEC_INTEGER) {
		++*ptr_to_tokvec;
		gen_push_int(tokvec[0].int_value);
		return;
	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		++*ptr_to_tokvec;
		printf("//`%s` as rvalue\n", tokvec[0].ident_str);
		gen_push_from_local(from_name(*ptr_ps, tokvec[0].ident_str));
		return;
	} else if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		*ptr_to_tokvec = tokvec;
		parse_expression(ptr_ps, &tokvec);
		if (tokvec[0].kind != RIGHT_PAREN) {

			error_unexpected_token(tokvec[0], "right paren");
		}
		++tokvec;
		*ptr_to_tokvec = tokvec;
		return;
	}

	error_unexpected_token(tokvec[0],
	                       "the beginning of parse_primary_expression");
}

void error_unexpected_token(struct Token token, const char *str)
{
	fprintf(stderr, "Unexpected token: `");
	print_token(token);
	fprintf(stderr, "` while expecting %s. Aborting.\n", str);
	exit(EXIT_FAILURE);
}

int get_label_name(struct ParserState *ptr_ps)
{
	return ++(ptr_ps->final_label_name);
}

void parse_statement(struct ParserState *ptr_ps,
                     const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == LEFT_BRACE) {
		parse_compound_statement(ptr_ps, &tokvec);
		*ptr_to_tokvec = tokvec;
	} else if (tokvec[0].kind == RES_IF) { /* or SWITCH */
		int label1 = get_label_name(ptr_ps);
		int label2 = get_label_name(ptr_ps);

		if (tokvec[1].kind != LEFT_PAREN) {
			error_unexpected_token(tokvec[1],
			                       "left parenthesis immediately after `if`");
		}
		tokvec += 2;
		parse_expression(ptr_ps, &tokvec);

		if (tokvec[0].kind != RIGHT_PAREN) {
			error_unexpected_token(tokvec[0], "right parenthesis of `if`");
		}
		++tokvec;

		gen_if_else_part1(label1, label2);

		parse_statement(ptr_ps, &tokvec);

		gen_if_else_part2(label1, label2);

		if (tokvec[0].kind == RES_ELSE) { /* must bind to the most inner one */
			++tokvec;
			parse_statement(ptr_ps, &tokvec);
		} else {
			/* do nothing */
		}

		gen_if_else_part3(label1, label2);

		*ptr_to_tokvec = tokvec;
	} else if (tokvec[0].kind == RES_RETURN) {
		++tokvec;
		*ptr_to_tokvec = tokvec;
		if (tokvec[0].kind == SEMICOLON) {
			assert("`return;` unimplemented" && 0);
		} else {
			parse_expression(ptr_ps, &tokvec);
			if (tokvec[0].kind != SEMICOLON) {
				error_unexpected_token(
				    tokvec[0],
				    "semicolon after `return` followed by an expression");
			}

			++tokvec;
			*ptr_to_tokvec = tokvec;

			/* the first occurrence of return within a function */
			if (ptr_ps->return_label_name == GARBAGE_INT) {
				int ret_label = get_label_name(ptr_ps);
				ptr_ps->return_label_name = ret_label;
				gen_return_with_label(ret_label);
			} else {
				gen_return_with_label(ptr_ps->return_label_name);
			}

			return;
		}
	} else if (tokvec[0].kind == RES_DO) {
		++tokvec;
		*ptr_to_tokvec = tokvec;
		int label1 = get_label_name(ptr_ps);
		gen_label(label1);
		parse_statement(ptr_ps, &tokvec);

		if (tokvec[0].kind != RES_WHILE) {
			error_unexpected_token(tokvec[0], "`while` of do-while");
		}

		++tokvec;

		if (tokvec[0].kind != LEFT_PAREN) {
			error_unexpected_token(tokvec[0], "left parenthesis of do-while");
		}

		++tokvec;
		*ptr_to_tokvec = tokvec;

		parse_expression(ptr_ps, &tokvec);

		if (tokvec[0].kind != RIGHT_PAREN) {
			error_unexpected_token(tokvec[0], "right parenthesis of do-while");
		}
		++tokvec;

		if (tokvec[0].kind != SEMICOLON) {
			error_unexpected_token(tokvec[0], "semicolon after do-while");
		}

		++tokvec;
		*ptr_to_tokvec = tokvec;

		gen_do_while_final(label1);

	} else if (tokvec[0].kind == RES_WHILE) {
		++tokvec;
		*ptr_to_tokvec = tokvec;

		if (tokvec[0].kind != LEFT_PAREN) {
			error_unexpected_token(tokvec[0], "left parenthesis of while");
		}

		++tokvec;

		int label1 = get_label_name(ptr_ps);
		int label2 = get_label_name(ptr_ps);

		gen_label(label1);

		parse_expression(ptr_ps, &tokvec);

		if (tokvec[0].kind != RIGHT_PAREN) {
			error_unexpected_token(tokvec[0], "left parenthesis of while");
		}

		++tokvec;

		gen_while_part2(label1, label2);

		parse_statement(ptr_ps, &tokvec);

		gen_while_part3(label1, label2);

		*ptr_to_tokvec = tokvec;

	} else {
		parse_expression(ptr_ps, &tokvec);
		if (tokvec[0].kind != SEMICOLON) {
			error_unexpected_token(tokvec[0], "semicolon after an expression");
		}
		gen_discard();
		++tokvec;
		*ptr_to_tokvec = tokvec;
		return;
	}
	*ptr_to_tokvec = tokvec;
}

void parse_final(const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind != END) {
		error_unexpected_token(tokvec[0], "the end of file");
	}
	return;
}

struct vector_Token read_all_tokens(const char *str)
{
	struct Token tok;
	struct vector_Token tokvec = init_vector_Token(0);
	while (1) {
		tok = get_token(&str);
		push_vector_Token(&tokvec, tok);
		if (tok.kind == END) {
			break;
		}
	}
	return tokvec;
}

void parse_compound_statement(struct ParserState *ptr_ps,
                              const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == LEFT_BRACE) {
		++tokvec;
		*ptr_to_tokvec = tokvec;
		while (1) {
			if (tokvec[0].kind == RIGHT_BRACE) {
				++tokvec;
				*ptr_to_tokvec = tokvec;
				return;
			} else {
				parse_statement(ptr_ps, &tokvec);
			}
		}
	}
}

void parse_function_definition(struct ParserState *ptr_ps,
                               const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == LEFT_PAREN) {
		const char *ident_str = tokvec[0].ident_str;

		int v = -4;
		struct int_map map = init_int_map();

		for (int i = 0;; i++) {
			if (tokvec[i].kind == END) {
				break;
			}
			if (tokvec[i].kind != IDENT_OR_RESERVED) {
				continue;
			}

			if (lookup(map, tokvec[i].ident_str) ==
			    GARBAGE_INT) { // newly found
				insert(&map, tokvec[i].ident_str, v);
				v -= 4;
			}
		}

		ptr_ps->var_table = map;
		ptr_ps->return_label_name = GARBAGE_INT; /* INITIALIZE */

		int capacity = -v - 4;

		if (tokvec[2].kind == RIGHT_PAREN) {
			gen_prologue(capacity, ident_str);
			tokvec += 3;
			parse_compound_statement(ptr_ps, &tokvec);
			gen_epilogue(ptr_ps->return_label_name);
		} else {

			gen_prologue(capacity, ident_str);

			tokvec += 2;

			const char *regs[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
			int counter = 0;

			if (tokvec[0].kind != IDENT_OR_RESERVED) {
				error_unexpected_token(tokvec[0],
				                       "identifier in the arglist of funcdef");
			}
			gen_write_register_to_local(
			    regs[counter], from_name(*ptr_ps, tokvec[0].ident_str));
			++counter;
			++tokvec;

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;

				if (tokvec[0].kind != IDENT_OR_RESERVED) {
					error_unexpected_token(
					    tokvec[0], "identifier in the arglist of funcdef");
				}

				if (counter > 5) {
					fprintf(stderr, "6-or-more args not implemented!\n");
				}
				gen_write_register_to_local(
				    regs[counter], from_name(*ptr_ps, tokvec[0].ident_str));
				++counter;
				++tokvec;
			}
			*ptr_to_tokvec = tokvec;

			if (tokvec[0].kind != RIGHT_PAREN) {
				error_unexpected_token(
				    tokvec[0], "closing parenthesis of function definition");
			}

			++tokvec;
			*ptr_to_tokvec = tokvec;

			parse_compound_statement(ptr_ps, &tokvec);
			gen_epilogue(ptr_ps->return_label_name);
		}
	} else {
		fprintf(stderr, "expected function definition but could not find it\n");
		fprintf(stderr, "current token: ");
		print_token(tokvec[0]);
		fprintf(stderr, "\nnext token: ");
		print_token(tokvec[1]);
		fprintf(stderr, "\n");
		exit(EXIT_FAILURE);
	}
	*ptr_to_tokvec = tokvec;
}

int main(int argc, char const *argv[])
{
	char str[1000];
	/* const char* str = "123+456-789"; */
	scanf("%[^\n]s", str); /* VULNERABLE!!! */
	if (argc == 2) {
		if (strcmp(argv[1], "--lexer-debug") == 0) {
			read_all_tokens_debug(str);
		}
	} else {
		struct vector_Token tokvec_ = read_all_tokens(str);
		const struct Token *tokvec = tokvec_.vector;

		struct ParserState ps;
		ps.final_label_name = 1;
		ps.return_label_name = GARBAGE_INT;

		while (1) {
			if (tokvec[0].kind == END) {
				parse_final(&tokvec);
				return 0;
			} else {
				parse_function_definition(&ps, &tokvec);
			}
		}
	}
	return 0;
}
