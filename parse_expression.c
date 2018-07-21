#include "header.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_additive_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec);
void parse_multiplicative_expression(struct ParserState *ptr_ps,
                                     const struct Token **ptr_tokvec);

void parse_exclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
void parse_OR_expression(struct ParserState *ptr_ps,
                         const struct Token **ptr_tokvec);
void parse_AND_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec);
void parse_equality_expression(struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec);
void parse_relational_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
void parse_shift_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec);

void binary_op(struct TypeCheckerState *ptr_tcs, enum TokenKind kind)
{
	switch (kind) {
		case OP_PLUS:
			typecheck_op_ints(ptr_tcs, "addl");
			return;
		case OP_MINUS:
			typecheck_op_ints(ptr_tcs, "subl");
			return;
		case OP_ASTERISK:
			typecheck_mul_ints(ptr_tcs);
			return;
		case OP_SLASH:
			typecheck_div_ints(ptr_tcs);
			return;
		case OP_PERCENT:
			typecheck_rem_ints(ptr_tcs);
			return;
		case OP_COMMA:
			typecheck_op_ints(ptr_tcs, "movl");
			return;
		case OP_LT:
			typecheck_compare_ints(ptr_tcs, "setl");
			return;
		case OP_LT_EQ:
			typecheck_compare_ints(ptr_tcs, "setle");
			return;
		case OP_LSHIFT:
			typecheck_shift_ints(ptr_tcs, "sall");
			return;
		case OP_GT:
			typecheck_compare_ints(ptr_tcs, "setg");
			return;
		case OP_GT_EQ:
			typecheck_compare_ints(ptr_tcs, "setge");
			return;
		case OP_RSHIFT:
			typecheck_shift_ints(ptr_tcs, "sarl");
			return;
		case OP_AND:
			typecheck_op_ints(ptr_tcs, "andl");
			return;
		case OP_OR:
			typecheck_op_ints(ptr_tcs, "orl");
			return;
		case OP_EQ_EQ:
			typecheck_compare_ints(ptr_tcs, "sete");
			return;
		case OP_NOT_EQ:
			typecheck_compare_ints(ptr_tcs, "setne");
			return;
		case OP_HAT:
			typecheck_op_ints(ptr_tcs, "xorl");
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
		struct TypeCheckerState tcs = ptr_ps->tcs;
		binary_op(&tcs, kind);
		ptr_ps->tcs = tcs;
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
		struct TypeCheckerState tcs = ptr_ps->tcs;
		binary_op(&tcs, kind);
		ptr_ps->tcs = tcs;
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
		struct TypeCheckerState tcs = ptr_ps->tcs;
		binary_op(&tcs, kind);
		ptr_ps->tcs = tcs;
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
		struct TypeCheckerState tcs = ptr_ps->tcs;
		binary_op(&tcs, kind);
		ptr_ps->tcs = tcs;
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
		struct TypeCheckerState tcs = ptr_ps->tcs;
		binary_op(&tcs, kind);
		ptr_ps->tcs = tcs;
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
		struct TypeCheckerState tcs = ptr_ps->tcs;
		binary_op(&tcs, kind);
		ptr_ps->tcs = tcs;
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
		struct TypeCheckerState tcs = ptr_ps->tcs;
		binary_op(&tcs, kind);
		ptr_ps->tcs = tcs;
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
		struct TypeCheckerState tcs = ptr_ps->tcs;
		binary_op(&tcs, kind);
		ptr_ps->tcs = tcs;
	}
	*ptr_tokvec = tokvec;
}
