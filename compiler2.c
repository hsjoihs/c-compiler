#include "header.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ParserState {
	struct int_map var_table;
	int final_label_name;
	int return_label_name;   /* the label at the end of the function */
	int break_label_name;    /* the label at the end of the current loop */
	int continue_label_name; /* the label at the beginning of the current loop
	                          */
};

struct VarInfo {
	int offset;
	int isDeclared;
};

void error_unexpected_token(const struct Token *tokvec, const char *str);
int get_new_label_name(struct ParserState *ptr_ps);
void expect_and_consume(const struct Token **ptr_tokvec, enum TokenKind kind,
                        const char *str);
void inc_or_dec(struct ParserState *ptr_ps, const char *name,
                enum TokenKind opkind);
void parse_type_name(struct ParserState *ptr_ps,
                     const struct Token **ptr_tokvec);

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

		default:
			assert("failure!!! not a binary op!!!!" && 0);
	}

	assert("unimplemented!!!!" && 0);
}

void read_all_tokens_debug(const char *str)
{
	struct Token tok;
	tok.kind = BEGINNING;
	tok.int_value = GARBAGE_INT;
	tok.ident_str = 0;

	print_token(tok);
	fprintf(stderr, "\n");

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
                      const struct Token **ptr_tokvec);
void parse_assignment_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
void parse_additive_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec);
void parse_multiplicative_expression(struct ParserState *ptr_ps,
                                     const struct Token **ptr_tokvec);
void parse_primary_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
void parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
void parse_exclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
void parse_AND_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec);
void parse_AND_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec);
void parse_equality_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec);
void parse_relational_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
void parse_shift_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec);
void parse_cast_expression(struct ParserState *ptr_ps,
                           const struct Token **ptr_tokvec);
void parse_unary_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec);
void parse_postfix_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
void parse_conditional_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec);
void parse_compound_statement(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
void parse_logical_OR_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
void parse_logical_AND_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec);

void parse_expression(struct ParserState *ptr_ps,
                      const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

struct VarInfo *from_name(struct ParserState ps, const char *str)
{
	if (!isElem(ps.var_table, str)) {
		assert("cannot happen" && 0);
	}

	struct VarInfo *info = lookup(ps.var_table, str);
	return info;
}

int get_offset_from_name(struct ParserState ps, const char *str)
{
	struct VarInfo *info = from_name(ps, str);
	if (!(info->isDeclared)) {
		fprintf(stderr, "%s is not declared\n", str);
	}
	return info->offset;
}

void before_assign(enum TokenKind kind)
{
	switch (kind) {
		case OP_PLUS_EQ:
		case OP_PLUS_PLUS:
			binary_op(OP_PLUS);
			return;
		case OP_MINUS_EQ:
		case OP_MINUS_MINUS:
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
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	enum TokenKind opkind = tokvec[1].kind;

	if (tokvec[0].kind == IDENT_OR_RESERVED &&
	    (opkind == OP_EQ || opkind == OP_PLUS_EQ || opkind == OP_MINUS_EQ ||
	     opkind == OP_ASTERISK_EQ || opkind == OP_SLASH_EQ ||
	     opkind == OP_PERCENT_EQ || opkind == OP_LSHIFT_EQ ||
	     opkind == OP_RSHIFT_EQ || opkind == OP_AND_EQ || opkind == OP_HAT_EQ ||
	     opkind == OP_OR_EQ)) {
		const char *name = tokvec[0].ident_str;
		tokvec += 2;
		*ptr_tokvec = tokvec;

		if (opkind != OP_EQ) {
			printf("//load from `%s`\n", name);
			gen_push_from_local(get_offset_from_name(*ptr_ps, name));
		}

		parse_assignment_expression(ptr_ps, &tokvec);

		if (opkind != OP_EQ) {
			printf("//before assigning to `%s`:\n", name);
			before_assign(opkind);
		}

		printf("//assign to `%s`\n", name);
		gen_write_to_local(get_offset_from_name(*ptr_ps, name));
	} else {
		parse_conditional_expression(ptr_ps, &tokvec);
	}
	*ptr_tokvec = tokvec;
}

void parse_conditional_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	parse_logical_OR_expression(ptr_ps, &tokvec);
	if (tokvec[0].kind == QUESTION) {
		int label1 = get_new_label_name(ptr_ps);
		int label2 = get_new_label_name(ptr_ps);

		gen_ternary_part1(label1, label2);
		++tokvec;
		*ptr_tokvec = tokvec;
		parse_expression(ptr_ps, &tokvec);

		gen_ternary_part2(label1, label2);

		expect_and_consume(&tokvec, COLON, "colon of the conditional operator");

		*ptr_tokvec = tokvec;
		parse_conditional_expression(ptr_ps, &tokvec);

		gen_ternary_part3(label1, label2);
	}
	*ptr_tokvec = tokvec;
}

void parse_logical_OR_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	int label1 = get_new_label_name(ptr_ps);
	int label2 = get_new_label_name(ptr_ps);

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

	*ptr_tokvec = tokvec;
}

void parse_logical_AND_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	int label1 = get_new_label_name(ptr_ps);
	int label2 = get_new_label_name(ptr_ps);

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

	*ptr_tokvec = tokvec;
}

void parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

void parse_exclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

void parse_AND_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

void parse_equality_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

void parse_relational_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

void parse_shift_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

void parse_additive_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

void parse_multiplicative_expression(struct ParserState *ptr_ps,
                                     const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
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
	*ptr_tokvec = tokvec;
}

void parse_cast_expression(struct ParserState *ptr_ps,
                           const struct Token **ptr_tokvec)
{
	parse_unary_expression(ptr_ps, ptr_tokvec);
}

void parse_type_name(struct ParserState *ptr_ps,
                     const struct Token **ptr_tokvec)
{
	expect_and_consume(ptr_tokvec, RES_INT, "type name `int`");
}

void parse_unary_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	/* unary-operator cast-expression */
	if (tokvec[0].kind == OP_NOT || tokvec[0].kind == OP_TILDA ||
	    tokvec[0].kind == OP_PLUS || tokvec[0].kind == OP_MINUS) {
		enum TokenKind kind = tokvec[0].kind;
		++tokvec;
		parse_cast_expression(ptr_ps, &tokvec);
		print_unary_prefix_op(kind);
	} else if ((tokvec[0].kind == OP_PLUS_PLUS ||
	            tokvec[0].kind == OP_MINUS_MINUS) &&
	           tokvec[1].kind == IDENT_OR_RESERVED) {
		const char *name = tokvec[1].ident_str;
		enum TokenKind opkind = tokvec[0].kind;
		tokvec += 2;
		*ptr_tokvec = tokvec;

		inc_or_dec(ptr_ps, name, opkind);

	} else {
		parse_postfix_expression(ptr_ps, &tokvec);
	}
	*ptr_tokvec = tokvec;
}

const char *get_reg_name_from_arg_pos(int counter)
{
	switch (counter) {
		case 0:
			return "edi";
		case 1:
			return "esi";
		case 2:
			return "edx";
		case 3:
			return "ecx";
		case 4:
			return "r8d";
		case 5:
			return "r9d";
		default:
			assert("cannot happen" && 0);
	}
}

void parse_postfix_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == LEFT_PAREN) {
		const char *ident_str = tokvec[0].ident_str;
		if (tokvec[2].kind == RIGHT_PAREN) {
			gen_push_ret_of(ident_str);
			tokvec += 3;
		} else {
			tokvec += 2;
			int counter = 0;

			parse_assignment_expression(ptr_ps, &tokvec);
			gen_pop_to_reg(get_reg_name_from_arg_pos(counter));
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
					exit(EXIT_FAILURE);
				}
				gen_pop_to_reg(get_reg_name_from_arg_pos(counter));
				counter++;
			}

			gen_push_ret_of(ident_str);
			*ptr_tokvec = tokvec;

			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing parenthesis of function call");

			*ptr_tokvec = tokvec;
		}

	} else if (tokvec[0].kind == IDENT_OR_RESERVED &&
	           (tokvec[1].kind == OP_PLUS_PLUS ||
	            tokvec[1].kind == OP_MINUS_MINUS)) {
		const char *name = tokvec[0].ident_str;
		enum TokenKind opkind = tokvec[1].kind;
		tokvec += 2;
		*ptr_tokvec = tokvec;

		inc_or_dec(ptr_ps, name, opkind);

		gen_push_int(-1);
		before_assign(opkind);
	} else {
		parse_primary_expression(ptr_ps, &tokvec);
	}
	*ptr_tokvec = tokvec;
}

/* returns the identifier */
const char *parse_declaration(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	parse_type_name(ptr_ps, &tokvec);
	expect_and_consume(&tokvec, IDENT_OR_RESERVED,
	                   "identifier as variable name");
	const char *str = tokvec[-1].ident_str;
	expect_and_consume(&tokvec, SEMICOLON,
	                   "semicolon at the end of variable definition");
	*ptr_tokvec = tokvec;

	return str;
}

void parse_primary_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LIT_DEC_INTEGER) {
		++*ptr_tokvec;
		gen_push_int(tokvec[0].int_value);
		return;
	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		++*ptr_tokvec;
		printf("//`%s` as rvalue\n", tokvec[0].ident_str);
		gen_push_from_local(get_offset_from_name(*ptr_ps, tokvec[0].ident_str));
		return;
	} else if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		parse_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "right paren");

		*ptr_tokvec = tokvec;
		return;
	}

	error_unexpected_token(tokvec, "the beginning of parse_primary_expression");
}

void error_unexpected_token(const struct Token *tokvec, const char *str)
{
	fprintf(stderr, "Unexpected token: `");
	print_token(tokvec[0]);
	fprintf(stderr, "` while expecting %s. \n", str);
	fprintf(stderr, "Next token: `");
	print_token(tokvec[1]);
	fprintf(stderr, "`\n");
	fprintf(stderr, "Previous token: `");
	print_token(
	    tokvec[-1]); /* it does not fail if tokvec[0] was the first token, since
	                    there always is at least one token (BEGINNING) */
	fprintf(stderr, "`\n");
	exit(EXIT_FAILURE);
}

int get_new_label_name(struct ParserState *ptr_ps)
{
	return ++(ptr_ps->final_label_name);
}

void expect_and_consume(const struct Token **ptr_tokvec, enum TokenKind kind,
                        const char *str)
{
	const struct Token *tokvec = *ptr_tokvec;

	if (tokvec[0].kind != kind) {
		error_unexpected_token(tokvec, str);
	}
	++tokvec;
	*ptr_tokvec = tokvec;
}

void parse_statement(struct ParserState *ptr_ps,
                     const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LEFT_BRACE) {
		parse_compound_statement(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
	} else if (tokvec[0].kind == RES_IF) { /* or SWITCH */
		int label1 = get_new_label_name(ptr_ps);
		int label2 = get_new_label_name(ptr_ps);
		++tokvec;

		expect_and_consume(&tokvec, LEFT_PAREN,
		                   "left parenthesis immediately after `if`");

		parse_expression(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, RIGHT_PAREN, "right parenthesis of `if`");

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

		*ptr_tokvec = tokvec;
	} else if (tokvec[0].kind == RES_RETURN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		if (tokvec[0].kind == SEMICOLON) {
			assert("`return;` unimplemented" && 0);
		} else {
			parse_expression(ptr_ps, &tokvec);

			expect_and_consume(
			    &tokvec, SEMICOLON,
			    "semicolon after `return` followed by an expression");

			*ptr_tokvec = tokvec;

			/* the first occurrence of return within a function */
			if (ptr_ps->return_label_name == GARBAGE_INT) {
				int ret_label = get_new_label_name(ptr_ps);
				ptr_ps->return_label_name = ret_label;
				gen_return_with_label(ret_label);
			} else {
				gen_return_with_label(ptr_ps->return_label_name);
			}

			return;
		}
	} else if (tokvec[0].kind == RES_DO) {

		int stashed_break_label = ptr_ps->break_label_name;
		int stashed_continue_label = ptr_ps->continue_label_name;

		++tokvec;
		*ptr_tokvec = tokvec;
		int label1 = get_new_label_name(ptr_ps);
		int break_label = get_new_label_name(ptr_ps);
		int cont_label = get_new_label_name(ptr_ps);

		ptr_ps->break_label_name = break_label;
		ptr_ps->continue_label_name = cont_label;

		gen_label(label1);
		parse_statement(ptr_ps, &tokvec);
		gen_label(cont_label);

		expect_and_consume(&tokvec, RES_WHILE, "`while` of do-while");
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of do-while");

		parse_expression(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "right parenthesis of do-while");
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after do-while");
		*ptr_tokvec = tokvec;

		gen_do_while_final(label1, break_label);

		ptr_ps->break_label_name = stashed_break_label;
		ptr_ps->continue_label_name = stashed_continue_label;

	} else if (tokvec[0].kind == RES_WHILE) {

		int stashed_break_label = ptr_ps->break_label_name;
		int stashed_continue_label = ptr_ps->continue_label_name;

		++tokvec;
		*ptr_tokvec = tokvec;

		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of while");

		int label1 = get_new_label_name(ptr_ps);
		int break_label = get_new_label_name(ptr_ps);
		int cont_label = get_new_label_name(ptr_ps);
		ptr_ps->break_label_name = break_label;
		ptr_ps->continue_label_name = cont_label;

		gen_label(label1);

		parse_expression(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, RIGHT_PAREN, "left parenthesis of while");

		gen_while_part2(label1, break_label);

		parse_statement(ptr_ps, &tokvec);

		gen_while_part3(label1, break_label, cont_label);

		*ptr_tokvec = tokvec;

		ptr_ps->break_label_name = stashed_break_label;
		ptr_ps->continue_label_name = stashed_continue_label;

	} else if (tokvec[0].kind == RES_BREAK) {

		++tokvec;
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after `break`");
		*ptr_tokvec = tokvec;

		if (ptr_ps->break_label_name == GARBAGE_INT) {
			fprintf(stderr, "invalid `break`; no loop, no switch\n");
			exit(EXIT_FAILURE);
		} else {
			printf("//break;\n");
			printf("  jmp .L%d\n", ptr_ps->break_label_name);
		}

		return;
	} else if (tokvec[0].kind == RES_CONTINUE) {

		++tokvec;
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after `continue`");
		*ptr_tokvec = tokvec;

		if (ptr_ps->continue_label_name == GARBAGE_INT) {
			fprintf(stderr, "invalid `continue`; no loop\n");
			exit(EXIT_FAILURE);
		} else {
			printf("//continue;\n");
			printf("  jmp .L%d\n", ptr_ps->continue_label_name);
		}

		return;

	} else if (tokvec[0].kind == RES_FOR) {
		int stashed_break_label = ptr_ps->break_label_name;
		int stashed_continue_label = ptr_ps->continue_label_name;
		int label1 = get_new_label_name(ptr_ps);
		int label2 = get_new_label_name(ptr_ps);
		int label3 = get_new_label_name(ptr_ps);
		ptr_ps->break_label_name = label2;
		ptr_ps->continue_label_name = label3;

		++tokvec;
		expect_and_consume(&tokvec, LEFT_PAREN, "left parenthesis of `for`");

		if (tokvec[0].kind == SEMICOLON) { /* expression1 is missing */
			;
			/* do nothing */
		} else {
			parse_expression(ptr_ps, &tokvec); /* expression1 */
			gen_discard();
		}

		expect_and_consume(&tokvec, SEMICOLON, "first semicolon of `for`");

		gen_label(label1);

		if (tokvec[0].kind == SEMICOLON) { /* expression2 is missing */
			gen_push_int(1);
		} else {
			parse_expression(ptr_ps, &tokvec); /* expression2 */
		}

		expect_and_consume(&tokvec, SEMICOLON, "second semicolon of `for`");

		gen_while_part2(label1, label2);

		if (tokvec[0].kind == RIGHT_PAREN) { /* expression3 is missing */
			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "right parenthesis of `for`");

			parse_statement(ptr_ps, &tokvec);

			gen_while_part3(label1, label2, label3);

			*ptr_tokvec = tokvec;

		} else {
			const struct Token *tokvec2 = tokvec;

			/* parse, but do not output. no problems regarding nested comments
			 * arises, since parse_expression can never have `for` within it */
			printf("/* commenting out, to handle expression3 of `for`\n");
			parse_expression(ptr_ps, &tokvec2);
			printf("*/\n");

			expect_and_consume(&tokvec2, RIGHT_PAREN,
			                   "right parenthesis of `for`");

			parse_statement(ptr_ps, &tokvec2);

			printf("//gen_for_part3(%d, %d, %d)\n", label1, label2, label3);
			printf(".L%d:\n", label3);

			printf("// what was previously ignored\n");

			parse_expression(ptr_ps, &tokvec);
			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "right parenthesis of `for`");
			gen_discard();

			printf("  jmp .L%d\n", label1);
			printf(".L%d:\n", label2);

			tokvec = tokvec2;
		}

		ptr_ps->break_label_name = stashed_break_label;
		ptr_ps->continue_label_name = stashed_continue_label;
	} else {
		parse_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, SEMICOLON, "semicolon after an expression");

		gen_discard();
		*ptr_tokvec = tokvec;
		return;
	}
	*ptr_tokvec = tokvec;
}

void parse_final(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	expect_and_consume(&tokvec, END, "the end of file");
	return;
}

int count_all_tokens(const char *str)
{
	struct Token tok;
	int count = 1;

	while (1) {
		tok = get_token(&str);
		++count;
		if (tok.kind == END) {
			break;
		}
	}
	return count;
}

struct Token *read_all_tokens_(const char *str)
{
	struct Token tok;
	int tok_num;
	{
		const char *str2 = str;
		tok_num = count_all_tokens(str2);
	}

	struct Token *tokvec = calloc(tok_num, sizeof(struct Token));

	tok.kind = BEGINNING;
	tok.int_value = GARBAGE_INT;
	tok.ident_str = 0;

	tokvec[0] = tok;
	int i = 0;
	while (1) {
		tok = get_token(&str);
		++i;
		tokvec[i] = tok;
		if (tok.kind == END) {
			break;
		}
	}
	return tokvec;
}

int can_start_a_type(const struct Token *tokvec)
{
	return tokvec[0].kind == RES_INT;
}

void parse_compound_statement(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LEFT_BRACE) {
		++tokvec;
		*ptr_tokvec = tokvec;
		while (1) {
			if (tokvec[0].kind == RIGHT_BRACE) {
				++tokvec;
				*ptr_tokvec = tokvec;
				return;
			} else if (can_start_a_type(tokvec)) {
				const char *str = parse_declaration(ptr_ps, &tokvec);
				struct VarInfo *info = from_name(*ptr_ps, str);
				info->isDeclared = 1;
			} else {
				parse_statement(ptr_ps, &tokvec);
			}
		}
	}
}

void parse_parameter_declaration(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec,
                                 int *ptr_counter)
{
	const struct Token *tokvec = *ptr_tokvec;
	int counter = *ptr_counter;
	if (tokvec[0].kind != IDENT_OR_RESERVED) {
		error_unexpected_token(tokvec, "identifier in the arglist of funcdef");
	}

	if (counter > 5) {
		fprintf(stderr, "6-or-more args not implemented!\n");
	}

	struct VarInfo *info = from_name(*ptr_ps, tokvec[0].ident_str);
	info->isDeclared = 1;

	gen_write_register_to_local(
	    get_reg_name_from_arg_pos(counter),
	    get_offset_from_name(*ptr_ps, tokvec[0].ident_str));
	++counter;
	++tokvec;
	*ptr_tokvec = tokvec;
	*ptr_counter = counter;
}

void parse_function_definition(struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == LEFT_PAREN) {
		const char *ident_str = tokvec[0].ident_str;

		int v = -4;
		struct int_map map = init_int_map();

		struct VarInfo *offset_vec = calloc(100, sizeof(struct VarInfo));

		int j = 0;
		for (int i = 0;; i++) {
			if (tokvec[i].kind == END) {
				break;
			}
			if (tokvec[i].kind != IDENT_OR_RESERVED) {
				continue;
			}

			if (!isElem(map, tokvec[i].ident_str)) { // newly found
				struct VarInfo info;
				info.offset = v;
				info.isDeclared = 0;
				offset_vec[j] = info;
				insert(&map, tokvec[i].ident_str, (void *)(&offset_vec[j]));
				j++;
				v -= 4;
			}
		}

		ptr_ps->var_table = map;
		ptr_ps->return_label_name = GARBAGE_INT; /* INITIALIZE */
		ptr_ps->break_label_name = GARBAGE_INT;  /* INITIALIZE */

		int capacity = -v - 4;

		if (tokvec[2].kind == RIGHT_PAREN) {
			gen_prologue(capacity, ident_str);
			tokvec += 3;
			parse_compound_statement(ptr_ps, &tokvec);
			gen_epilogue(ptr_ps->return_label_name);
		} else {

			gen_prologue(capacity, ident_str);

			tokvec += 2;

			int counter = 0;

			parse_parameter_declaration(ptr_ps, &tokvec, &counter);

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;

				parse_parameter_declaration(ptr_ps, &tokvec, &counter);
			}
			*ptr_tokvec = tokvec;

			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing parenthesis of function definition");

			*ptr_tokvec = tokvec;

			parse_compound_statement(ptr_ps, &tokvec);
			gen_epilogue(ptr_ps->return_label_name);
		}
	} else {
		fprintf(stderr, "expected function definition but could not find it\n");
		fprintf(stderr, "current token: ");
		print_token(tokvec[0]);
		fprintf(stderr, "\nnext token: ");
		print_token(tokvec[1]);
		fprintf(stderr, "\nprevious token: ");
		print_token(tokvec[-1]);
		fprintf(stderr, "\n");
		exit(EXIT_FAILURE);
	}
	*ptr_tokvec = tokvec;
}

void inc_or_dec(struct ParserState *ptr_ps, const char *name,
                enum TokenKind opkind)
{
	printf("//load from `%s`\n", name);
	gen_push_from_local(get_offset_from_name(*ptr_ps, name));
	gen_push_int(1);

	printf("//before assigning to `%s`:\n", name);
	before_assign(opkind);

	printf("//assign to `%s`\n", name);
	gen_write_to_local(get_offset_from_name(*ptr_ps, name));
}

int main(int argc, char const **argv)
{
	char *str;

	str = calloc(10000, sizeof(char));

	/* const char* str = "123+456-789"; */
	scanf("%[^\n]s", str); /* VULNERABLE!!! */
	if (argc == 2) {
		if (strcmp(argv[1], "--lexer-debug") == 0) {
			read_all_tokens_debug(str);
		}
	} else {
		const struct Token *tokvec = read_all_tokens_(str);

		++tokvec; /* skip the dummy token BEGINNING */

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
