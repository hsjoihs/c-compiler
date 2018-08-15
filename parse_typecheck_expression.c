#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static struct UntypedExpression
parse_unary_expression(const struct ParserState *ptr_ps,
                       const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_conditional_expression(const struct ParserState *ptr_ps,
                             const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_logical_OR_expression(const struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_AND_expression(const struct ParserState *ptr_ps,
                     const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_exclusive_OR_expression(const struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
static struct UntypedExpression
parse_equality_expression(const struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec);
static struct UntypedExpression
deref_expr_untyped(struct UntypedExpression expr);
static struct UntypedExpression
binary_op_untyped(struct UntypedExpression expr, struct UntypedExpression expr2,
                  enum TokenKind kind)
{
	struct UntypedExpression *ptr_expr1 =
	    calloc(1, sizeof(struct UntypedExpression));
	struct UntypedExpression *ptr_expr2 =
	    calloc(1, sizeof(struct UntypedExpression));
	*ptr_expr1 = expr;
	*ptr_expr2 = expr2;

	struct UntypedExpression new_expr;
	new_expr.operator= kind;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct Expression
parse_typecheck_cast_expression(const struct ParserState *ptr_ps,
                                const struct Token **ptr_tokvec);

struct UntypedExpression NOTHING = {0, NOINFO};

static enum SimpleBinOp to_simplebinop(enum TokenKind t)
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
parse_typecheck_inclusive_OR_expression(const struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_additive_expression(const struct ParserState *ptr_ps,
                                    const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_multiplicative_expression(const struct ParserState *ptr_ps,
                                          const struct Token **ptr_tokvec);

static struct Expression
parse_typecheck_exclusive_OR_expression(const struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_AND_expression(const struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_equality_expression(const struct ParserState *ptr_ps,
                                    const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_relational_expression(const struct ParserState *ptr_ps,
                                      const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_shift_expression(const struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);

static struct Expression
parse_typecheck_logical_AND_expression(const struct ParserState *ptr_ps,
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

static struct Expression simple_binary_op(struct Expression expr,
                                          struct Expression expr2,
                                          enum TokenKind kind, struct Type type)
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

static struct Expression pointer_plusorminus_int(struct Expression expr,
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

static struct UntypedExpression
parse_inclusive_OR_expression(const struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr =
	    parse_exclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 =
		    parse_exclusive_OR_expression(ptr_ps, &tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_typecheck_inclusive_OR_expression(const struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr =
	    parse_typecheck_exclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_exclusive_OR_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE, "left operand of bitOR");
		expect_type(expr2.details.type, INT_TYPE, "right operand of bitOR");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_exclusive_OR_expression(const struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr = parse_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_HAT) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_AND_expression(ptr_ps, &tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

static struct Expression
parse_typecheck_exclusive_OR_expression(const struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Expression expr = parse_typecheck_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_HAT) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_AND_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE, "left operand of XOR");
		expect_type(expr2.details.type, INT_TYPE, "right operand of XOR");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

static struct UntypedExpression
parse_AND_expression(const struct ParserState *ptr_ps,
                     const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr = parse_equality_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 =
		    parse_equality_expression(ptr_ps, &tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

static struct Expression
parse_typecheck_AND_expression(const struct ParserState *ptr_ps,
                               const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr =
	    parse_typecheck_equality_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_equality_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE, "left operand of bitAND");
		expect_type(expr2.details.type, INT_TYPE, "right operand of bitAND");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;

	return expr;
}

static struct UntypedExpression
parse_cast_expression(const struct ParserState *ptr_ps,
                      const struct Token **ptr_tokvec)
{
	return parse_unary_expression(ptr_ps, ptr_tokvec);
}

static struct UntypedExpression
parse_multiplicative_expression(const struct ParserState *ptr_ps,
                                const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr = parse_cast_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_ASTERISK && kind != OP_SLASH && kind != OP_PERCENT) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 = parse_cast_expression(ptr_ps, &tokvec);

		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_additive_expression(const struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr =
	    parse_multiplicative_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_PLUS && kind != OP_MINUS) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 =
		    parse_multiplicative_expression(ptr_ps, &tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_shift_expression(const struct ParserState *ptr_ps,
                       const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr = parse_additive_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_LSHIFT && kind != OP_RSHIFT) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 =
		    parse_additive_expression(ptr_ps, &tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_relational_expression(const struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr = parse_shift_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_GT && kind != OP_GT_EQ && kind != OP_LT &&
		    kind != OP_LT_EQ) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 =
		    parse_shift_expression(ptr_ps, &tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_equality_expression(const struct ParserState *ptr_ps,
                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr =
	    parse_relational_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_EQ_EQ && kind != OP_NOT_EQ) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 =
		    parse_relational_expression(ptr_ps, &tokvec);
		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_typecheck_equality_expression(const struct ParserState *ptr_ps,
                                    const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr =
	    parse_typecheck_relational_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_EQ_EQ && kind != OP_NOT_EQ) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_relational_expression(ptr_ps, &tokvec);
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
parse_typecheck_relational_expression(const struct ParserState *ptr_ps,
                                      const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_typecheck_shift_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_GT && kind != OP_GT_EQ && kind != OP_LT &&
		    kind != OP_LT_EQ) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_shift_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "left operand of a relational operator");
		expect_type(expr2.details.type, INT_TYPE,
		            "right operand of a relational operator");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_typecheck_shift_expression(const struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr =
	    parse_typecheck_additive_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_LSHIFT && kind != OP_RSHIFT) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_additive_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "left operand of a shift operator");
		expect_type(expr2.details.type, INT_TYPE,
		            "left operand of a shift operator");
		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression combine_by_add(struct Expression expr,
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
parse_typecheck_additive_expression(const struct ParserState *ptr_ps,
                                    const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr =
	    parse_typecheck_multiplicative_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_PLUS && kind != OP_MINUS) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_multiplicative_expression(ptr_ps, &tokvec);
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
parse_typecheck_multiplicative_expression(const struct ParserState *ptr_ps,
                                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr = parse_typecheck_cast_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_ASTERISK && kind != OP_SLASH && kind != OP_PERCENT) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_cast_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "left operand of a multiplicative operator");
		expect_type(expr2.details.type, INT_TYPE,
		            "right operand of a multiplicative operator");

		expr = simple_binary_op(expr, expr2, kind, INT_TYPE);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct UntypedExpression
parse_logical_AND_expression(const struct ParserState *ptr_ps,
                             const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	int counter = 0;

	struct UntypedExpression first_expr =
	    parse_inclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND_AND) {
			break;
		}

		++tokvec;

		struct UntypedExpression expr2 =
		    parse_inclusive_OR_expression(ptr_ps, &tokvec);
		++counter;

		first_expr = binary_op_untyped(first_expr, expr2, OP_AND_AND);
	}

	*ptr_tokvec = tokvec;
	return first_expr;
}

static struct Expression
parse_typecheck_logical_AND_expression(const struct ParserState *ptr_ps,
                                       const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	int counter = 0;
	struct Expression first_expr =
	    parse_typecheck_inclusive_OR_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_AND_AND) {
			break;
		}

		++tokvec;
		struct Expression expr2 =
		    parse_typecheck_inclusive_OR_expression(ptr_ps, &tokvec);
		++counter;

		first_expr = binary_op(first_expr, expr2, LOGICAL_AND_EXPR, INT_TYPE);
	}

	*ptr_tokvec = tokvec;
	return first_expr;
}

static struct UntypedExpression
parse_logical_OR_expression(const struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct UntypedExpression expr =
	    parse_logical_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR_OR) {
			break;
		}

		++tokvec;
		struct UntypedExpression expr2 =
		    parse_logical_AND_expression(ptr_ps, &tokvec);

		expr = binary_op_untyped(expr, expr2, OP_OR_OR);
	}

	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_typecheck_logical_OR_expression(const struct ParserState *ptr_ps,
                                      const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Expression expr =
	    parse_typecheck_logical_AND_expression(ptr_ps, &tokvec);

	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_OR_OR) {
			break;
		}

		++tokvec;
		struct Expression expr2 =
		    parse_typecheck_logical_AND_expression(ptr_ps, &tokvec);

		expr = binary_op(expr, expr2, LOGICAL_OR_EXPR, INT_TYPE);
	}

	*ptr_tokvec = tokvec;
	return expr;
}

static int is_local_var(struct ScopeChain t, const char *str)
{
	if (isElem(t.var_table, str)) {
		return 1;
	} else if (t.outer == 0) {
		/* most outer, but cannot be found */
		return 0;
	} else {
		return is_local_var(*(t.outer), str);
	}
}

static struct LocalVarInfo resolve_name_locally(struct ScopeChain t,
                                                const char *str)
{
	if (isElem(t.var_table, str)) {
		struct LocalVarInfo *ptr_varinfo = lookup(t.var_table, str);
		return *ptr_varinfo;
	} else if (t.outer == 0) {
		/* most outer, but cannot be found */
		fprintf(stderr, "%s is not declared locally\n", str);
		exit(EXIT_FAILURE);
	} else {
		return resolve_name_locally(*(t.outer), str);
	}
}

static int isAssign(enum TokenKind opkind)
{
	return (opkind == OP_EQ || opkind == OP_PLUS_EQ || opkind == OP_MINUS_EQ ||
	        opkind == OP_ASTERISK_EQ || opkind == OP_SLASH_EQ ||
	        opkind == OP_PERCENT_EQ || opkind == OP_LSHIFT_EQ ||
	        opkind == OP_RSHIFT_EQ || opkind == OP_AND_EQ ||
	        opkind == OP_HAT_EQ || opkind == OP_OR_EQ);
}

static struct Type resolve_name_globally(struct Map m, const char *str)
{
	if (isElem(m, str)) {
		struct Type *ptr_type = lookup(m, str);
		return *ptr_type;
	} else {
		fprintf(stderr, "%s is not declared globally\n", str);
		exit(EXIT_FAILURE);
	}
}

static enum UnaryOp to_unaryop(enum TokenKind t)
{
	switch (t) {
		case OP_NOT:
			return UNARY_OP_NOT;
		case OP_TILDA:
			return UNARY_OP_TILDA;
		case OP_PLUS:
			return UNARY_OP_PLUS;
		case OP_MINUS:
			return UNARY_OP_MINUS;

		case OP_PLUS_PLUS:
			return UNARY_OP_PLUS_PLUS;
		case OP_MINUS_MINUS:
			return UNARY_OP_MINUS_MINUS;

		case OP_AND:
			return UNARY_OP_AND;

		case OP_ASTERISK:
			return UNARY_OP_ASTERISK;

		default:
			assert("cannot happen" && 0);
	}
}

static struct Expression
parse_typecheck_postfix_expression(const struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_conditional_expression(const struct ParserState *ptr_ps,
                                       const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_primary_expression(const struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
static struct Expression
parse_typecheck_assignment_expression(const struct ParserState *ptr_ps,
                                      const struct Token **ptr_tokvec);

static struct Expression deref_expr(struct Expression expr);

static struct Expression unary_op_(struct Expression expr, enum TokenKind kind,
                                   struct Type type)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;

	struct Expression new_expr;
	new_expr.details.type = type;
	new_expr.category = UNARY_OP_EXPR;
	new_expr.unary_operator = to_unaryop(kind);
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = 0;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct UntypedExpression unary_op_untyped(struct UntypedExpression expr,
                                                 enum TokenKind kind)
{
	struct UntypedExpression *ptr_expr1 =
	    calloc(1, sizeof(struct UntypedExpression));
	*ptr_expr1 = expr;

	struct UntypedExpression new_expr;
	new_expr.category = UNARY_EXPR;
	new_expr.operator= kind;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = 0;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct Expression
parse_typecheck_unary_expression(const struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	/* unary-operator cast-expression */
	if (tokvec[0].kind == OP_NOT || tokvec[0].kind == OP_TILDA ||
	    tokvec[0].kind == OP_PLUS || tokvec[0].kind == OP_MINUS) {
		enum TokenKind kind = tokvec[0].kind;
		++tokvec;
		struct Expression expr =
		    parse_typecheck_cast_expression(ptr_ps, &tokvec);
		expect_type(
		    expr.details.type, INT_TYPE,
		    "operand of logical not, bitnot, unary plus or unary minus");

		struct Expression new_expr = unary_op_(expr, kind, expr.details.type);
		new_expr.details.true_type = expr.details.true_type;

		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_PLUS_PLUS ||
	           tokvec[0].kind == OP_MINUS_MINUS) {
		enum TokenKind opkind = tokvec[0].kind;
		++tokvec;

		struct Expression expr =
		    parse_typecheck_unary_expression(ptr_ps, &tokvec);
		expect_type(expr.details.type, INT_TYPE,
		            "operand of unary increment/decrement");

		struct Expression new_expr = unary_op_(expr, opkind, INT_TYPE);
		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_AND) {
		tokvec++;

		struct Expression expr;

		expr = parse_typecheck_cast_expression(ptr_ps, &tokvec);

		struct Type type = expr.details.type;
		if (is_array(type)) {
			fprintf(stderr, "array is not an lvalue\n");
			exit(EXIT_FAILURE);
		}

		struct Type *ptr_type = calloc(1, sizeof(struct Type));
		*ptr_type = expr.details.type;

		struct Expression new_expr =
		    unary_op_(expr, OP_AND, ptr_of_type_to_ptr_to_type(ptr_type));

		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_ASTERISK) {
		++tokvec;
		struct Expression expr =
		    parse_typecheck_cast_expression(ptr_ps, &tokvec);

		*ptr_tokvec = tokvec;
		return deref_expr(expr);
	} else {
		struct Expression expr =
		    parse_typecheck_postfix_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr;
	}
}

static struct UntypedExpression
parse_unary_expression(const struct ParserState *ptr_ps,
                       const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	/* unary-operator cast-expression */
	if (tokvec[0].kind == OP_NOT || tokvec[0].kind == OP_TILDA ||
	    tokvec[0].kind == OP_PLUS || tokvec[0].kind == OP_MINUS) {
		enum TokenKind kind = tokvec[0].kind;
		++tokvec;

		struct UntypedExpression expr = parse_cast_expression(ptr_ps, &tokvec);

		struct UntypedExpression new_expr = unary_op_untyped(expr, kind);

		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_PLUS_PLUS ||
	           tokvec[0].kind == OP_MINUS_MINUS) {
		enum TokenKind opkind = tokvec[0].kind;
		++tokvec;

		struct UntypedExpression expr = parse_unary_expression(ptr_ps, &tokvec);

		struct UntypedExpression new_expr = unary_op_untyped(expr, opkind);
		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_AND) {
		tokvec++;

		struct UntypedExpression expr = parse_cast_expression(ptr_ps, &tokvec);

		struct UntypedExpression new_expr = unary_op_untyped(expr, OP_AND);

		*ptr_tokvec = tokvec;
		return new_expr;
	} else if (tokvec[0].kind == OP_ASTERISK) {
		++tokvec;
		struct UntypedExpression expr = parse_cast_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return deref_expr_untyped(expr);
	} else {
		struct Expression expr__ =
		    parse_typecheck_postfix_expression(ptr_ps, &tokvec);
		struct UntypedExpression expr = NOTHING;
		*ptr_tokvec = tokvec;
		return expr;
	}
}

static struct UntypedExpression
deref_expr_untyped(struct UntypedExpression expr)
{

	struct UntypedExpression new_expr = unary_op_untyped(expr, OP_ASTERISK);

	return new_expr;
}

static struct Expression deref_expr(struct Expression expr)
{
	struct Type type = deref_type(expr.details.type);

	struct Expression new_expr =
	    unary_op_(expr, OP_ASTERISK, if_array_convert_to_ptr(type));
	new_expr.details.true_type = type;

	return new_expr;
}

static struct Expression
parse_typecheck_cast_expression(const struct ParserState *ptr_ps,
                                const struct Token **ptr_tokvec)
{
	return parse_typecheck_unary_expression(ptr_ps, ptr_tokvec);
}

static struct Expression
parse_typecheck_postfix_expression(const struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == IDENT_OR_RESERVED && tokvec[1].kind == LEFT_PAREN) {
		const char *ident_str = tokvec[0].ident_str;

		struct Type ret_type;
		if (!isElem(ptr_ps->func_info_map, ident_str)) {
			fprintf(stderr, "Undeclared function `%s()` detected.\n",
			        ident_str);
			fprintf(stderr, "Assumes that `%s()` returns `int`\n", ident_str);
			ret_type = INT_TYPE;
		} else {
			struct FuncInfo *ptr_func_info =
			    lookup(ptr_ps->func_info_map, ident_str);
			ret_type = ptr_func_info->ret_type;
		}

		tokvec += 2;

		struct Expression *args = calloc(10, sizeof(struct Expression));
		int counter = 0;
		if (tokvec[0].kind == RIGHT_PAREN) {
			tokvec++;
		} else {
			args[counter] =
			    parse_typecheck_assignment_expression(ptr_ps, &tokvec);
			++counter;

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;
				if (counter > 5) {
					unsupported("calling with 7 or more arguments");
				}

				args[counter] =
				    parse_typecheck_assignment_expression(ptr_ps, &tokvec);
				++counter;
			}

			*ptr_tokvec = tokvec;

			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing parenthesis of function call");

			*ptr_tokvec = tokvec;
		}
		*ptr_tokvec = tokvec;

		struct Expression expr;
		expr.category = FUNCCALL_EXPR;
		expr.arg_expr_vec = args;
		expr.arg_length = counter;
		expr.details.type = ret_type;
		expr.global_var_name = ident_str;
		return expr;
	}

	struct Expression expr =
	    parse_typecheck_primary_expression(ptr_ps, &tokvec);
	while (1) {
		if (tokvec[0].kind == LEFT_BRACKET) {
			++tokvec;
			struct Expression expr2 =
			    parse_typecheck_expression(ptr_ps, &tokvec);
			expect_and_consume(&tokvec, RIGHT_BRACKET, "right bracket ]");

			expr = deref_expr(combine_by_add(expr, expr2));
		} else if (tokvec[0].kind == OP_PLUS_PLUS ||
		           tokvec[0].kind == OP_MINUS_MINUS) {
			enum TokenKind opkind = tokvec[0].kind;
			tokvec++;

			struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
			*ptr_expr1 = expr;

			struct Expression new_expr;
			new_expr.details.type = INT_TYPE;
			new_expr.category =
			    opkind == OP_PLUS_PLUS ? POSTFIX_INCREMENT : POSTFIX_DECREMENT;
			new_expr.ptr1 = ptr_expr1;
			new_expr.ptr2 = 0;
			new_expr.ptr3 = 0;

			expr = new_expr;
		} else {
			break;
		}
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_typecheck_primary_expression(const struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	if (tokvec[0].kind == LIT_DEC_INTEGER) {
		++*ptr_tokvec;

		struct Expression expr;
		expr.details.type = INT_TYPE;
		expr.int_value = tokvec[0].int_value;
		expr.category = INT_VALUE;

		return expr;

	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		++*ptr_tokvec;

		if (!is_local_var(ptr_ps->scope_chain, tokvec[0].ident_str)) {
			struct Type type = resolve_name_globally(
			    ptr_ps->global_vars_type_map, tokvec[0].ident_str);

			struct Expression expr;
			expr.details.type = if_array_convert_to_ptr(type);
			;
			expr.details.true_type = type;
			expr.category = GLOBAL_VAR_;
			expr.global_var_name = tokvec[0].ident_str;
			return expr;
		} else {
			struct LocalVarInfo info =
			    resolve_name_locally(ptr_ps->scope_chain, tokvec[0].ident_str);

			struct Expression expr;
			expr.details.type = if_array_convert_to_ptr(info.type);
			expr.details.true_type = info.type;
			expr.category = LOCAL_VAR_;
			expr.local_var_offset = info.offset;
			return expr;
		}
	} else if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		*ptr_tokvec = tokvec;
		struct Expression expr = parse_typecheck_expression(ptr_ps, &tokvec);
		expect_and_consume(&tokvec, RIGHT_PAREN, "right paren");

		*ptr_tokvec = tokvec;
		return expr;
	} else if (tokvec[0].kind == LIT_STRING) {
		struct Type *ptr_char = calloc(1, sizeof(struct Type));
		*ptr_char = CHAR_TYPE;

		struct Expression expr;
		expr.details.type = ptr_of_type_to_ptr_to_type(ptr_char);
		expr.details.true_type = ptr_of_type_to_arr_of_type(
		    ptr_char, strlen(tokvec[0].literal_str) + 1);
		expr.category = STRING_LITERAL;
		expr.literal_string = tokvec[0].literal_str;

		++*ptr_tokvec;
		return expr;
	}

	error_unexpected_token(
	    tokvec, "the beginning of parse_typecheck_primary_expression");
}

static enum SimpleBinOp op_before_assign(enum TokenKind kind)
{
	switch (kind) {
		case OP_EQ:
			return SIMPLE_BIN_OP_COMMA;
		case OP_PLUS_EQ:
			return SIMPLE_BIN_OP_PLUS;
		case OP_MINUS_EQ:
			return SIMPLE_BIN_OP_MINUS;
		case OP_ASTERISK_EQ:
			return SIMPLE_BIN_OP_ASTERISK;
		case OP_SLASH_EQ:
			return SIMPLE_BIN_OP_SLASH;
		case OP_PERCENT_EQ:
			return SIMPLE_BIN_OP_PERCENT;
		case OP_LSHIFT_EQ:
			return SIMPLE_BIN_OP_LSHIFT;
		case OP_RSHIFT_EQ:
			return SIMPLE_BIN_OP_RSHIFT;
		case OP_AND_EQ:
			return SIMPLE_BIN_OP_AND;
		case OP_HAT_EQ:
			return SIMPLE_BIN_OP_HAT;
		case OP_OR_EQ:
			return SIMPLE_BIN_OP_OR;
		default:
			assert("op_before_assign called before a non-assignment operator" &&
			       0);
	}
}

static struct Expression assignment_expr(struct Expression expr,
                                         struct Expression expr2,
                                         enum TokenKind opkind)
{
	struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
	struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
	*ptr_expr1 = expr;
	*ptr_expr2 = expr2;

	struct Expression new_expr;
	new_expr.details = expr.details;
	new_expr.category = ASSIGNMENT_EXPR;
	new_expr.simple_binary_operator = op_before_assign(opkind);
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct UntypedExpression
parse_assignment_expression(const struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	const struct Token *tokvec2 = tokvec;
	const struct ParserState *ptr_ps2 = ptr_ps;
	struct Expression expr__ =
	    parse_typecheck_unary_expression(ptr_ps2, &tokvec2);
	struct UntypedExpression expr = NOTHING;

	/* parse failed */
	if (!isAssign(tokvec2[0].kind)) {
		struct UntypedExpression expr_ =
		    parse_conditional_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr_;
	}

	tokvec = tokvec2;

	assert(isAssign(tokvec[0].kind));
	enum TokenKind opkind = tokvec[0].kind;
	++tokvec;

	struct UntypedExpression expr2 =
	    parse_assignment_expression(ptr_ps, &tokvec);

	*ptr_tokvec = tokvec;
	return binary_op_untyped(expr, expr2, opkind);
}

static struct Expression
parse_typecheck_assignment_expression(const struct ParserState *ptr_ps,
                                      const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	const struct Token *tokvec2 = tokvec;
	const struct ParserState *ptr_ps2 = ptr_ps;
	struct Expression expr =
	    parse_typecheck_unary_expression(ptr_ps2, &tokvec2);

	/* parse failed */
	if (!isAssign(tokvec2[0].kind)) {
		struct Expression expr_ =
		    parse_typecheck_conditional_expression(ptr_ps, &tokvec);
		*ptr_tokvec = tokvec;
		return expr_;
	}

	tokvec = tokvec2;

	if (is_array(expr.details.type) || is_array(expr.details.true_type)) {
		fprintf(stderr, "array is not an lvalue\n");
		exit(EXIT_FAILURE);
	}

	assert(isAssign(tokvec[0].kind));
	enum TokenKind opkind = tokvec[0].kind;
	++tokvec;

	struct Expression expr2 =
	    parse_typecheck_assignment_expression(ptr_ps, &tokvec);
	expect_type(expr.details.type, expr2.details.type,
	            "mismatch in assignment operator");

	*ptr_tokvec = tokvec;
	return assignment_expr(expr, expr2, opkind);
}

static struct UntypedExpression
parse_conditional_expression(const struct ParserState *ptr_ps,
                             const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr =
	    parse_logical_OR_expression(ptr_ps, &tokvec);
	if (tokvec[0].kind == QUESTION) {

		++tokvec;
		*ptr_tokvec = tokvec;
		struct UntypedExpression true_branch =
		    parse_expression(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, COLON, "colon of the conditional operator");

		*ptr_tokvec = tokvec;
		struct UntypedExpression false_branch =
		    parse_conditional_expression(ptr_ps, &tokvec);

		*ptr_tokvec = tokvec;

		struct UntypedExpression *ptr_expr1 =
		    calloc(1, sizeof(struct UntypedExpression));
		struct UntypedExpression *ptr_expr2 =
		    calloc(1, sizeof(struct UntypedExpression));
		struct UntypedExpression *ptr_expr3 =
		    calloc(1, sizeof(struct UntypedExpression));
		*ptr_expr1 = expr;
		*ptr_expr2 = true_branch;
		*ptr_expr3 = false_branch;

		struct UntypedExpression new_expr;
		new_expr.category = CONDITIONAL;
		new_expr.ptr1 = ptr_expr1;
		new_expr.ptr2 = ptr_expr2;
		new_expr.ptr3 = ptr_expr3;

		return new_expr;
	}
	*ptr_tokvec = tokvec;
	return expr;
}

static struct Expression
parse_typecheck_conditional_expression(const struct ParserState *ptr_ps,
                                       const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr =
	    parse_typecheck_logical_OR_expression(ptr_ps, &tokvec);
	if (tokvec[0].kind == QUESTION) {

		++tokvec;
		*ptr_tokvec = tokvec;
		struct Expression true_branch =
		    parse_typecheck_expression(ptr_ps, &tokvec);

		expect_and_consume(&tokvec, COLON, "colon of the conditional operator");

		*ptr_tokvec = tokvec;
		struct Expression false_branch =
		    parse_typecheck_conditional_expression(ptr_ps, &tokvec);

		*ptr_tokvec = tokvec;

		expect_type(false_branch.details.type, true_branch.details.type,
		            "mismatch of type in the false branch and the true branch");

		struct Expression *ptr_expr1 = calloc(1, sizeof(struct Expression));
		struct Expression *ptr_expr2 = calloc(1, sizeof(struct Expression));
		struct Expression *ptr_expr3 = calloc(1, sizeof(struct Expression));
		*ptr_expr1 = expr;
		*ptr_expr2 = true_branch;
		*ptr_expr3 = false_branch;

		struct Expression new_expr;
		new_expr.details = true_branch.details;
		new_expr.category = CONDITIONAL_EXPR;
		new_expr.ptr1 = ptr_expr1;
		new_expr.ptr2 = ptr_expr2;
		new_expr.ptr3 = ptr_expr3;

		return new_expr;
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct UntypedExpression parse_expression(const struct ParserState *ptr_ps,
                                          const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct UntypedExpression expr =
	    parse_assignment_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_COMMA) {
			break;
		}
		++tokvec;

		struct UntypedExpression expr2 =
		    parse_assignment_expression(ptr_ps, &tokvec);

		expr = binary_op_untyped(expr, expr2, kind);
	}
	*ptr_tokvec = tokvec;
	return expr;
}

struct Expression parse_typecheck_expression(const struct ParserState *ptr_ps,
                                             const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;
	struct Expression expr =
	    parse_typecheck_assignment_expression(ptr_ps, &tokvec);
	while (1) {
		enum TokenKind kind = tokvec[0].kind;
		if (kind != OP_COMMA) {
			break;
		}
		++tokvec;

		struct Expression expr2 =
		    parse_typecheck_assignment_expression(ptr_ps, &tokvec);

		expr = simple_binary_op(expr, expr2, kind, expr2.details.type);
	}
	*ptr_tokvec = tokvec;
	return expr;
}
