#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum SimpleBinOp to_simplebinop(enum TokenKind t)
{
	switch (t) {
		case OP_PLUS:
			return SIMPLE_BIN_OP_PLUS;
		case OP_MINUS:
			return SIMPLE_BIN_OP_MINUS;
		case OP_ASTERISK:
			return SIMPLE_BIN_OP_ASTERISK;
		case OP_SLASH:
			return SIMPLE_BIN_OP_SLASH;
		case OP_PERCENT:
			return SIMPLE_BIN_OP_PERCENT;
		case OP_COMMA:
			return SIMPLE_BIN_OP_COMMA;
		case OP_LT:
			return SIMPLE_BIN_OP_LT;
		case OP_LT_EQ:
			return SIMPLE_BIN_OP_LT_EQ;
		case OP_LSHIFT:
			return SIMPLE_BIN_OP_LSHIFT;
		case OP_GT:
			return SIMPLE_BIN_OP_GT;
		case OP_GT_EQ:
			return SIMPLE_BIN_OP_GT_EQ;
		case OP_RSHIFT:
			return SIMPLE_BIN_OP_RSHIFT;
		case OP_AND:
			return SIMPLE_BIN_OP_AND;
		case OP_OR:
			return SIMPLE_BIN_OP_OR;
		case OP_EQ_EQ:
			return SIMPLE_BIN_OP_EQ_EQ;
		case OP_NOT_EQ:
			return SIMPLE_BIN_OP_NOT_EQ;
		case OP_HAT:
			return SIMPLE_BIN_OP_HAT;
		default:
			assert("cannot happen" && 0);
	}
}

static struct Expression
parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
static struct Expression
parse_additive_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec);
static struct Expression
parse_multiplicative_expression(struct ParserState *ptr_ps,
                                const struct Token **ptr_tokvec);

static struct Expression
parse_exclusive_OR_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
static struct Expression parse_AND_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec);
static struct Expression
parse_equality_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec);
static struct Expression
parse_relational_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec);
static struct Expression
parse_shift_expression(struct ParserState *ptr_ps,
                       const struct Token **ptr_tokvec);

static struct Expression
parse_logical_AND_expression(struct ParserState *ptr_ps,
                             const struct Token **ptr_tokvec);

static struct Expression binary_op(struct Expression expr,
                                   struct Expression expr2,
                                   enum ExprCategory cat, struct Type type)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;
	*ptr_expr2 = expr2;

	struct Expression new_expr;
	new_expr.details.type = type;
	new_expr.category = cat;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

struct Expression simple_binary_op(struct Expression expr,
                                   struct Expression expr2, enum TokenKind kind,
                                   struct Type type)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;
	*ptr_expr2 = expr2;

	struct Expression new_expr;
	new_expr.details.type = type;
	new_expr.category = SIMPLE_BINARY_EXPR;
	new_expr.simple_binary_operator = to_simplebinop(kind);
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

struct Expression pointer_plusorminus_int(struct Expression expr,
                                          struct Expression expr2,
                                          enum TokenKind kind)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;
	*ptr_expr2 = expr2;

	struct Expression new_expr;
	new_expr.details = expr.details;
	new_expr.category = kind == OP_PLUS ? POINTER_PLUS_INT : POINTER_MINUS_INT;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct Expression
parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_exclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_exclusive_OR_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE, "left operand of bitOR");
		expect_type(expr2.details.type, INT_TYPE, "right operand of bitOR");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_exclusive_OR_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Expression expr = parse_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_HAT) {
			break;
		}
		++tokvec;

		struct Expression expr2 = parse_AND_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE, "left operand of XOR");
		expect_type(expr2.details.type, INT_TYPE, "right operand of XOR");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

static struct Expression parse_AND_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_equality_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND) {
			break;
		}
		++tokvec;

		struct Expression expr2 = parse_equality_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE, "left operand of bitAND");
		expect_type(expr2.details.type, INT_TYPE, "right operand of bitAND");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

static struct Expression
parse_equality_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_relational_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_EQ_EQ && kind != OP_NOT_EQ) {
			break;
		}
		++tokvec;

		struct Expression expr2 = parse_relational_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "left operand of an equality operator");
		expect_type(expr2.details.type, INT_TYPE,
		            "right operand of an equality operator");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_relational_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_shift_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_GT && kind != OP_GT_EQ && kind != OP_LT &&
		    kind != OP_LT_EQ) {
			break;
		}
		++tokvec;

		struct Expression expr2 = parse_shift_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "left operand of a relational operator");
		expect_type(expr2.details.type, INT_TYPE,
		            "right operand of a relational operator");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression parse_shift_expression(struct ParserState *ptr_ps,
                                                const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_additive_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_LSHIFT && kind != OP_RSHIFT) {
			break;
		}
		++tokvec;

		struct Expression expr2 = parse_additive_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "left operand of a shift operator");
		expect_type(expr2.details.type, INT_TYPE,
		            "left operand of a shift operator");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct Expression combine_by_add(struct Expression expr,
                                 struct Expression expr2)
{
	struct Type type1 = expr.details.type;
	struct Type type2 = expr2.details.type;

	if (is_compatible(type1, INT_TYPE)) {
		if (is_compatible(type2, INT_TYPE)) {
			return simple_binary_op(expr, expr2, OP_PLUS, INT_TYPE);
		} else if (is_pointer(type2)) {
			/* swapped */
			return pointer_plusorminus_int(expr2, expr, OP_PLUS);
		}

	} else if (is_pointer(type1)) {

		expect_type(expr2.details.type, INT_TYPE,
		            "cannot add a pointer to a pointer");
		return pointer_plusorminus_int(expr, expr2, OP_PLUS);
	}
	fprintf(stderr, "fail\n");
	exit(EXIT_FAILURE);
}

static struct Expression combine_by_sub(struct Expression expr,
                                        struct Expression expr2)
{
	struct Type type1 = expr.details.type;
	struct Type type2 = expr2.details.type;

	if (is_compatible(type1, INT_TYPE)) {
		if (is_compatible(type2, INT_TYPE)) {
			return simple_binary_op(expr, expr2, OP_MINUS, INT_TYPE);
		} else if (is_pointer(type2)) {

			fprintf(stderr, "cannot subtract a pointer from an integer.\n");
			exit(EXIT_FAILURE);
		}

	} else if (is_pointer(type1)) {
		if (is_compatible(type2, INT_TYPE)) {

			/* pointer minus int */
			return pointer_plusorminus_int(expr, expr2, OP_MINUS);
		} else {
			/* pointer minus pointer */
			return binary_op(expr, expr2, POINTER_MINUS_POINTER, INT_TYPE);
		}
	}
	fprintf(stderr, "fail\n");
	exit(EXIT_FAILURE);
}

static struct Expression
parse_additive_expression(struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_multiplicative_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_PLUS && kind != OP_MINUS) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_multiplicative_expression(ptr_ps, &tokvec);
		if (kind == OP_PLUS) {
			expr = combine_by_add(expr, expr2);
		} else {
			expr = combine_by_sub(expr, expr2);
		}
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_multiplicative_expression(struct ParserState *ptr_ps,
                                const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_cast_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_ASTERISK && kind != OP_SLASH && kind != OP_PERCENT) {
			break;
		}
		++tokvec;

		struct Expression expr2 = parse_cast_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "left operand of a multiplicative operator");
		expect_type(expr2.details.type, INT_TYPE,
		            "right operand of a multiplicative operator");

		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_logical_AND_expression(struct ParserState *ptr_ps,
                             const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	int counter = 0;
	struct Expression first_expr =
	    parse_inclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND_AND) {
			break;
		}

		++tokvec;
		struct Expression expr2 =
		    parse_inclusive_OR_expression(ptr_ps, &tokvec);
		++counter;

		first_expr = binary_op(first_expr, expr2, LOGICAL_AND_EXPR, INT_TYPE);
	}

	*ptr_tokvec = tokvec;
	return first_expr;
}

struct Expression parse_logical_OR_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Expression expr = parse_logical_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR_OR) {
			break;
		}

		++tokvec;
		struct Expression expr2 = parse_logical_AND_expression(ptr_ps, &tokvec);

		expr = binary_op(expr, expr2, LOGICAL_OR_EXPR, INT_TYPE);
	}

	*ptr_tokvec = tokvec;
	return expr;
}
