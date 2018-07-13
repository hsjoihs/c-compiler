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

void print_unary_prefix_op(enum TokenKind kind)
{
	switch (kind) {
		case OP_NOT:
			unary_not();
			return;
		case OP_TILDA:
			unary("notl");
			return;
		case OP_PLUS:
			/* do nothing */
			return;
		case OP_MINUS:
			unary("negl");
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

void print_op(enum TokenKind kind)
{
	switch (kind) {
		case OP_PLUS:
			op_ints("addl");
			return;
		case OP_MINUS:
			op_ints("subl");
			return;
		case OP_ASTERISK:
			mul_ints();
			return;
		case OP_SLASH:
			div_ints();
			return;
		case OP_PERCENT:
			rem_ints();
			return;
		case OP_COMMA:
			op_ints("movl");
			return;
		case OP_LT:
			compare_ints("setl");
			return;
		case OP_LT_EQ:
			compare_ints("setle");
			return;
		case OP_LSHIFT:
			shift_ints("sall");
			return;
		case OP_GT:
			compare_ints("setg");
			return;
		case OP_GT_EQ:
			compare_ints("setge");
			return;
		case OP_RSHIFT:
			shift_ints("sarl");
			return;
		case OP_AND:
			op_ints("andl");
			return;
		case OP_OR:
			op_ints("orl");
			return;
		case OP_EQ_EQ:
			compare_ints("sete");
			return;
		case OP_NOT_EQ:
			compare_ints("setne");
			return;

		case OP_EQ: {
			assert("= is unimplemented!!!!" && 0);
			return;
		}

		case OP_NOT:
		case OP_TILDA:
		case IDENT_OR_RESERVED:
		case LEFT_PAREN:
		case RIGHT_PAREN:
		case END:
		case LIT_DEC_INTEGER:
		case RES_RETURN:
		case SEMICOLON:
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
		print_op(OP_COMMA);
	}
	*ptr_to_tokvec = tokvec;
}

int from_name(struct ParserState ps, const char *str)
{
	return lookup(ps.var_table, str);
}

void parse_assignment_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == OP_EQ) {
		const char *name = tokvec[0].ident_str;
		tokvec += 2;
		*ptr_to_tokvec = tokvec;
		parse_assignment_expression(ptr_ps, &tokvec);

		printf("//assign to `%s`\n", name);
		write_to_local(from_name(*ptr_ps, name));
	} else {
		parse_conditional_expression(ptr_ps, &tokvec);
	}
	*ptr_to_tokvec = tokvec;
}

/* FIXME */
void parse_conditional_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_to_tokvec)
{
	parse_inclusive_OR_expression(ptr_ps, ptr_to_tokvec);
}

void parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	parse_AND_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR) {
			break;
		}
		++tokvec;
		parse_AND_expression(ptr_ps, &tokvec);
		print_op(kind);
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
		print_op(kind);
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
		print_op(kind);
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
		print_op(kind);
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
		print_op(kind);
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
		print_op(kind);
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
		print_op(kind);
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
	parse_primary_expression(ptr_ps, ptr_to_tokvec);
}

void parse_primary_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == LIT_DEC_INTEGER) {
		++*ptr_to_tokvec;
		push_int(tokvec[0].int_value);
		return;
	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		++*ptr_to_tokvec;
		printf("//`%s` as rvalue\n", tokvec[0].ident_str);
		push_from_local(from_name(*ptr_ps, tokvec[0].ident_str));
		return;
	} else if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		*ptr_to_tokvec = tokvec;
		parse_expression(ptr_ps, &tokvec);
		if (tokvec[0].kind == RIGHT_PAREN) {
			++tokvec;
			*ptr_to_tokvec = tokvec;
			return;
		} else {
			error_unexpected_token(tokvec[0], "right paren");
		}
	}

	error_unexpected_token(tokvec[0],
	                       "the beginning of parse_primary_expression");
}

void error_unexpected_token(struct Token token, const char *str)
{
	fprintf(stderr, "Unexpected token: `");
	print_token(token);
	fprintf(stderr, "` while expecting %s. Aborting.\n", str);
	abort();
}

void parse_statement(struct ParserState *ptr_ps,
                     const struct Token **ptr_to_tokvec)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == RES_RETURN) {
		++tokvec;
		*ptr_to_tokvec = tokvec;
		if (tokvec[0].kind == SEMICOLON) {
			assert("`return;` unimplemented" && 0);
		} else {
			parse_expression(ptr_ps, &tokvec);
			if (tokvec[0].kind == SEMICOLON) {
				++tokvec;
				*ptr_to_tokvec = tokvec;

				return_with_label("FIXME");

				return;
			} else {
				error_unexpected_token(
				    tokvec[0],
				    "semicolon after `return` followed by an expression");
			}
		}
	} else {
		parse_expression(ptr_ps, &tokvec);
		if (tokvec[0].kind == SEMICOLON) {
			print_op(OP_COMMA); /* like the comma operator, discard what's on
			                       the stack */
			++tokvec;
			*ptr_to_tokvec = tokvec;
			return;
		} else {
			error_unexpected_token(tokvec[0], "semicolon after an expression");
		}
	}
	*ptr_to_tokvec = tokvec;
}

void parse_final(struct ParserState *ptr_ps, const struct Token **ptr_to_tokvec,
                 int offset)
{
	const struct Token *tokvec = *ptr_to_tokvec;
	if (tokvec[0].kind == END) {
		print_epilogue("FIXME", offset);
	}
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

		int v = -4;
		struct int_map map = init_int_map();

		for (int i = 0; i < tokvec_.length; i++) {
			if (tokvec_.vector[i].kind != IDENT_OR_RESERVED) {
				continue;
			}

			if (lookup(map, tokvec_.vector[i].ident_str) ==
			    GARBAGE_INT) { // newly found
				insert(&map, tokvec_.vector[i].ident_str, v);
				v -= 4;
			}
		}

		struct ParserState ps;
		ps.var_table = map;
		ps.final_label_name = 0;
		ps.return_label_name = GARBAGE_INT;

		int capacity = -v - 4;
		print_prologue(capacity);
		while (1) {
			if (tokvec[0].kind == END) {
				parse_final(&ps, &tokvec, capacity);
				return 0;
			} else {
				parse_statement(&ps, &tokvec);
			}
		}
	}
	return 0;
}
