#include "header.h"
#include "print_x86_64.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Expression wrap(struct ExprInfo info)
{
	struct Expression expr;
	expr.category = UNKNOWN;
	expr.details = info;
	return expr;
}

struct Expression remove_leftiness_(struct Expression expr)
{
	expr.details = remove_leftiness(expr.details);
	return expr;
}

struct Expression parse_additive_expression(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec);
struct ExprInfo
parse_multiplicative_expression(struct ParserState *ptr_ps,
                                const struct Token **ptr_tokvec);

struct Expression
parse_exclusive_OR_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
struct ExprInfo parse_OR_expression(struct ParserState *ptr_ps,
                                    const struct Token **ptr_tokvec);
struct Expression parse_AND_expression(struct ParserState *ptr_ps,
                                       const struct Token **ptr_tokvec);
struct Expression parse_equality_expression(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec);
struct Expression parse_relational_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec);
struct Expression parse_shift_expression(struct ParserState *ptr_ps,
                                         const struct Token **ptr_tokvec);

struct ExprInfo parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info =
	    parse_exclusive_OR_expression(ptr_ps, &tokvec).details;

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR) {
			break;
		}
		++tokvec;

		expect_type(expr_info, INT_TYPE, 3);

		expr_info = remove_leftiness(
		    parse_exclusive_OR_expression(ptr_ps, &tokvec).details);
		expect_type(expr_info, INT_TYPE, 4);
		// print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}

struct Expression binary_op_(struct Expression expr, struct Expression expr2,
                             enum TokenKind kind, enum expr_category cat,
                             struct ExprInfo exprinfo)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;
	*ptr_expr2 = expr2;

	struct Expression new_expr;
	new_expr.details = exprinfo;
	new_expr.category = cat;
	new_expr.binary_operator = kind;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

struct Expression binary_op(struct Expression expr, struct Expression expr2,
                            enum TokenKind kind)
{
	return binary_op_(expr, expr2, kind, BINARY_EXPR, UNASSIGNABLE(INT_TYPE));
}

struct Expression pointer_plusorminus_int(struct Expression expr,
                                          struct Expression expr2,
                                          enum TokenKind kind)
{
	return binary_op_(expr, expr2, kind, POINTER_PLUSORMINUS_INT, expr.details);
}

struct Expression parse_exclusive_OR_expression(struct ParserState *ptr_ps,
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

		struct Expression expr2 =
		    remove_leftiness_(parse_AND_expression(ptr_ps, &tokvec));
		expect_type(expr.details, INT_TYPE, 5);
		expect_type(expr2.details, INT_TYPE, 6);
		expr = binary_op(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

struct Expression parse_AND_expression(struct ParserState *ptr_ps,
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

		struct Expression expr2 =
		    remove_leftiness_(parse_equality_expression(ptr_ps, &tokvec));
		expect_type(expr.details, INT_TYPE, 7);
		expect_type(expr2.details, INT_TYPE, 8);
		expr = binary_op(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

struct Expression parse_equality_expression(struct ParserState *ptr_ps,
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

		struct Expression expr2 =
		    remove_leftiness_(parse_relational_expression(ptr_ps, &tokvec));
		expect_type(expr.details, INT_TYPE, 9);
		expect_type(expr2.details, INT_TYPE, 10);
		expr = binary_op(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct Expression parse_relational_expression(struct ParserState *ptr_ps,
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

		struct Expression expr2 =
		    remove_leftiness_(parse_shift_expression(ptr_ps, &tokvec));
		expect_type(expr.details, INT_TYPE, 11);
		expect_type(expr2.details, INT_TYPE, 12);
		expr = binary_op(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct Expression parse_shift_expression(struct ParserState *ptr_ps,
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

		struct Expression expr2 =
		    remove_leftiness_(parse_additive_expression(ptr_ps, &tokvec));
		expect_type(expr.details, INT_TYPE, 13);
		expect_type(expr2.details, INT_TYPE, 14);
		expr = binary_op(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct Expression parse_additive_expression(struct ParserState *ptr_ps,
                                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr =
	    wrap(parse_multiplicative_expression(ptr_ps, &tokvec));

	struct Type type1 = expr.details.type;

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_PLUS && kind != OP_MINUS) {
			break;
		}
		++tokvec;

		struct Expression expr2 = wrap(
		    remove_leftiness(parse_multiplicative_expression(ptr_ps, &tokvec)));
		struct Type type2 = expr2.details.type;

		if (is_equal(type1, INT_TYPE)) {
			if (is_equal(type2, INT_TYPE)) {
				expr = binary_op(expr, expr2, kind);
			} else if (is_pointer(type2)) {
				if (kind == OP_MINUS) {
					fprintf(stderr,
					        "cannot subtract a pointer from an integer.\n");
					exit(EXIT_FAILURE);
				}

				/* swapped */
				expr = pointer_plusorminus_int(expr2, expr, kind);
			}

		} else if (is_pointer(type1)) {
			// int size = size_of(deref_type(expr_info.type));
			if (kind == OP_PLUS) {
				expect_type(expr2.details, INT_TYPE, 30);
				/* cannot add a pointer to a pointer*/

				expr = pointer_plusorminus_int(expr, expr2, kind);
			} else if (is_equal(type2, INT_TYPE)) {

				/* pointer minus int */
				expr = pointer_plusorminus_int(expr, expr2, kind);
			} else {
				/* pointer minus pointer */
				expr = binary_op_(expr, expr2, kind, POINTER_MINUS_POINTER,
				                  UNASSIGNABLE(INT_TYPE));
			}
		}
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct ExprInfo parse_multiplicative_expression(struct ParserState *ptr_ps,
                                                const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct ExprInfo expr_info = parse_cast_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_ASTERISK && kind != OP_SLASH && kind != OP_PERCENT) {
			break;
		}
		++tokvec;

		struct ExprInfo expr_info2 =
		    remove_leftiness(parse_cast_expression(ptr_ps, &tokvec));
		expect_type(expr_info, INT_TYPE, 17);
		expect_type(expr_info2, INT_TYPE, 18);
		expr_info2 = expr_info;
		// print_binary_op(kind);
	}
	*ptr_tokvec = tokvec;
	return expr_info;
}
