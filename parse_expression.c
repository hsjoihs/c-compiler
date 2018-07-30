#include "header.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ExprInfo parseprint_additive_expression(struct ParserState *ptr_ps,
                                               const struct Token **ptr_tokvec);
struct ExprInfo
parseprint_multiplicative_expression(struct ParserState *ptr_ps,
                                     const struct Token **ptr_tokvec);

struct ExprInfo
parseprint_exclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
struct ExprInfo parseprint_OR_expression(struct ParserState *ptr_ps,
                                         const struct Token **ptr_tokvec);
struct ExprInfo parseprint_AND_expression(struct ParserState *ptr_ps,
                                          const struct Token **ptr_tokvec);
struct ExprInfo parseprint_equality_expression(struct ParserState *ptr_ps,
                                               const struct Token **ptr_tokvec);
struct ExprInfo
parseprint_relational_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
struct ExprInfo parseprint_shift_expression(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec);

void print_binary_op(enum TokenKind kind)
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
			gen_discard2nd_8byte();
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

struct ExprInfo
parseprint_inclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info =
	    parseprint_exclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR) {
			break;
		}
		++tokvec;

		expect_type(expr_info, INT_TYPE, 3);

		expr_info = remove_leftiness(
		    parseprint_exclusive_OR_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 4);
		print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}

struct ExprInfo
parseprint_exclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info = parseprint_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_HAT) {
			break;
		}
		++tokvec;

		expect_type(expr_info, INT_TYPE, 5);

		expr_info =
		    remove_leftiness(parseprint_AND_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 6);
		print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}

struct ExprInfo parseprint_AND_expression(struct ParserState *ptr_ps,
                                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info = parseprint_equality_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND) {
			break;
		}
		++tokvec;

		expect_type(expr_info, INT_TYPE, 7);

		expr_info =
		    remove_leftiness(parseprint_equality_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 8);
		print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}

struct ExprInfo parseprint_equality_expression(struct ParserState *ptr_ps,
                                               const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info =
	    parseprint_relational_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_EQ_EQ && kind != OP_NOT_EQ) {
			break;
		}
		++tokvec;

		expect_type(expr_info, INT_TYPE, 9);

		expr_info =
		    remove_leftiness(parseprint_relational_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 10);
		print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}

struct ExprInfo
parseprint_relational_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info = parseprint_shift_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_GT && kind != OP_GT_EQ && kind != OP_LT &&
		    kind != OP_LT_EQ) {
			break;
		}
		++tokvec;

		expect_type(expr_info, INT_TYPE, 11);

		expr_info =
		    remove_leftiness(parseprint_shift_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 12);
		print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}

struct ExprInfo parseprint_shift_expression(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info = parseprint_additive_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_LSHIFT && kind != OP_RSHIFT) {
			break;
		}
		++tokvec;

		expect_type(expr_info, INT_TYPE, 13);

		expr_info =
		    remove_leftiness(parseprint_additive_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 14);
		print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}

struct ExprInfo parseprint_additive_expression(struct ParserState *ptr_ps,
                                               const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info =
	    parseprint_multiplicative_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_PLUS && kind != OP_MINUS) {
			break;
		}
		++tokvec;

		struct ExprInfo expr_info2 = remove_leftiness(
		    parseprint_multiplicative_expression(ptr_ps, &tokvec));
		if (is_equal(expr_info.type, INT_TYPE)) {
			if (is_equal(expr_info2.type, INT_TYPE)) {
				expr_info2 = expr_info;
				print_binary_op(kind);
				expr_info = remove_leftiness(expr_info); /* int */
			} else if (is_pointer(expr_info2.type)) {
				if (kind == OP_MINUS) {
					fprintf(stderr,
					        "cannot subtract a pointer from an integer.\n");
					exit(EXIT_FAILURE);
				}
				int size = size_of(deref_type(expr_info2.type));
				gen_swap();
				gen_cltq();
				gen_mul_by_const(size);

				gen_op_8byte("addq");
				expr_info = expr_info2; /* pointer */
			}

		} else if (is_pointer(expr_info.type)) {
			int size = size_of(deref_type(expr_info.type));
			if (kind == OP_PLUS) {
				expect_type(expr_info2, INT_TYPE, 30);
				/* cannot add a pointer to a pointer*/
				gen_cltq();
				gen_mul_by_const(size);

				gen_op_8byte("addq");
				expr_info = remove_leftiness(expr_info); /* pointer */
			} else if (is_equal(expr_info2.type, INT_TYPE)) {
				gen_cltq();
				gen_mul_by_const(size);

				gen_op_8byte("subq");
				expr_info = remove_leftiness(expr_info); /* pointer */
			} else {
				/* subtract pointer */
				gen_op_8byte("subq");
				gen_div_by_const(size);
				expr_info = UNASSIGNABLE(INT_TYPE);
			}
		}
	}
	*ptr_tokvec = tokvec;
	return expr_info; /* pointer */
}

struct ExprInfo
parseprint_multiplicative_expression(struct ParserState *ptr_ps,
                                     const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info = parseprint_cast_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_ASTERISK && kind != OP_SLASH && kind != OP_PERCENT) {
			break;
		}
		++tokvec;

		struct ExprInfo expr_info2 =
		    remove_leftiness(parseprint_cast_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 17);
		expect_type(expr_info2, INT_TYPE, 18);
		expr_info2 = expr_info;
		print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}
