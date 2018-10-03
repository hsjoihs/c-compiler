#include "analyzer.h"
#include "std.h"
#include "std_io.h"

static const struct EnumeratorAndValue *
get_global_enumerator(const struct Vector /*<EnumeratorAndValue>*/ *ref_list,
                      const char *name);

static int is_local_var(const struct ScopeChain *t, const char *str);

int typecheck_constant_integral_expression(struct AnalyzerState *ptr_ps,
                                           const struct UntypedExpr *ref_uexpr,
                                           const char *context)
{
	const struct UntypedExpr uexpr = *ref_uexpr;
	switch (uexpr.category) {
		case INT_LITERAL_:
			return uexpr.int_value;
		case VAR: {
			const char *name = uexpr.var_name;

			if (is_local_var(&ptr_ps->scope_chain, name)) {
				break;
			}

			const struct EnumeratorAndValue *ptr_enum_and_value =
			    get_global_enumerator(&ptr_ps->global_enumerator_list, name);

			if (!ptr_enum_and_value) {
				break;
			}

			return ptr_enum_and_value->value;
		}
		default:
			break;
	}

	fprintf(stderr, "Expected const expression, but did not get one.\n");
	fprintf(stderr, "context: %s\n", context);
	exit(EXIT_FAILURE);
}

static int is_compatible(const struct AnalyzerState *ptr_ps,
                         const struct Type *ref_t1, const struct Type *ref_t2);

void expect_type(const struct AnalyzerState *ptr_ps,
                 const struct Type *ref_actual_type,
                 const struct Type *ref_expected_type, const char *message)
{

	if (!is_compatible(ptr_ps, ref_actual_type, ref_expected_type)) {
		fprintf(stderr, "Unmatched type: expected `");
		debug_print_type(ref_expected_type);
		fprintf(stderr, "`, but got `");
		debug_print_type(ref_actual_type);
		fprintf(stderr, "`.\n");
		fprintf(stderr, "context: %s\n", message);
		exit(EXIT_FAILURE);
	}
}

static int is_strictly_equal(const struct AnalyzerState *ptr_ps,
                             const struct Type *ref_t1,
                             const struct Type *ref_t2)
{
	const struct Type t1 = *ref_t1;
	const struct Type t2 = *ref_t2;
	if (t1.type_category == INT_ && t2.type_category == INT_) {
		return 1;
	}

	if (t1.type_category == CHAR_ && t2.type_category == CHAR_) {
		return 1;
	}

	if (t1.type_category == VOID_ && t2.type_category == VOID_) {
		return 1;
	}

	if (t1.type_category == PTR_ && t2.type_category == PTR_) {

		/* pointer to incomplete types */
		if (t1.derived_from->type_category == STRUCT_ &&
		    t2.derived_from->type_category == STRUCT_ &&
		    strcmp(t1.derived_from->s.struct_tag,
		           t2.derived_from->s.struct_tag) == 0) {
			return 1;
		}

		return is_strictly_equal(ptr_ps, t1.derived_from, t2.derived_from);
	}

	if (t1.type_category == ARRAY && t2.type_category == ARRAY) {
		return is_strictly_equal(ptr_ps, t1.derived_from, t2.derived_from) &&
		       (t1.array_length == t2.array_length);
	}

	if (t1.type_category == STRUCT_ && t2.type_category == STRUCT_) {
		if ((0)) { /* both are local */
			unsupported("struct type declared locally");
		}
		return strcmp(t1.s.struct_tag, t2.s.struct_tag) == 0 &&
		       lookup(ptr_ps->global_struct_tag_map, t1.s.struct_tag);
	}

	if (t1.type_category == ENUM_ && t2.type_category == ENUM_) {
		if ((0)) { /* both are local */
			unsupported("enum type declared locally");
		}
		return strcmp(t1.e.enum_tag, t2.e.enum_tag) == 0;
	}

	return 0;
}

static int is_integral(const struct Type *ref_t1)
{
	return ref_t1->type_category == INT_ || ref_t1->type_category == CHAR_ ||
	       ref_t1->type_category == ENUM_;
}

static int is_scalar(const struct Type *ref_t1)
{
	return ref_t1->type_category == PTR_ || is_integral(ref_t1);
}

void expect_scalar(const struct Type *ref_type, const char *context)
{
	if (!is_scalar(ref_type)) {
		fprintf(stderr, "Expected a scalar type, but got a non-scalar type `");
		debug_print_type(ref_type);
		fprintf(stderr, "`.\ncontext: %s\n", context);
		exit(EXIT_FAILURE);
	}
}

void expect_integral(const struct Type *ref_actual_type, const char *message)
{

	if (!is_integral(ref_actual_type)) {
		fprintf(stderr, "Unmatched type: expected an integral type, but got a "
		                "non-integral type `");
		debug_print_type(ref_actual_type);
		fprintf(stderr, "`.\n");
		fprintf(stderr, "context: %s\n", message);
		exit(EXIT_FAILURE);
	}
}

static int is_compatible(const struct AnalyzerState *ptr_ps,
                         const struct Type *ref_t1, const struct Type *ref_t2)
{
	if (is_strictly_equal(ptr_ps, ref_t1, ref_t2)) {
		return 1;
	}

	if (is_integral(ref_t1) && is_integral(ref_t2)) {
		return 1;
	}

	const struct Type t1 = *ref_t1;
	const struct Type t2 = *ref_t2;

	if (t1.type_category == PTR_ && t2.type_category == PTR_) {
		return t1.derived_from->type_category == VOID_ ||
		       t2.derived_from->type_category == VOID_;
	}

	if (t1.type_category == PTR_ && t2.type_category == ARRAY) {
		struct Type t3 = t2; /* copy of t2 */
		t3.type_category = PTR_;
		return is_compatible(ptr_ps, &t1, &t3);
	}

	if (t1.type_category == ARRAY && t2.type_category == PTR_) {
		return is_compatible(ptr_ps, &t2, &t1);
	}

	if (t1.type_category == FN && t2.type_category == FN) {
		if (!is_strictly_equal(ptr_ps, t1.derived_from, t2.derived_from)) {
			return 0;
		}

		if (!t1.is_param_infos_valid || !t2.is_param_infos_valid) {
			return 1;
		}

		if (t1.param_infos.length != t2.param_infos.length) {
			return 0;
		}

		for (int i = 0; i < t1.param_infos.length; i++) {
			const struct TypeAndIdent *ptr_ti1 = t1.param_infos.vector[i];
			const struct TypeAndIdent *ptr_ti2 = t2.param_infos.vector[i];

			if (!is_strictly_equal(ptr_ps, &ptr_ti1->type, &ptr_ti2->type)) {
				return 0;
			}
		}
		return 1;
	}

	return 0;
}

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
			assert0("cannot happen" && 0);
	}
}

static struct Expr binary_op(const struct Expr *ref_expr,
                             const struct Expr *ref_expr2,
                             enum ExprCategory cat, const struct Type *ref_type)
{
	struct Expr *ptr_expr1 = calloc(1, sizeof(struct Expr));
	struct Expr *ptr_expr2 = calloc(1, sizeof(struct Expr));
	*ptr_expr1 = *ref_expr;
	*ptr_expr2 = *ref_expr2;

	struct Expr new_expr;
	new_expr.details.type = *ref_type;
	new_expr.details.true_type = *ref_type;
	new_expr.category = cat;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct Expr simple_binary_op(const struct Expr *ref_expr,
                                    const struct Expr *ref_expr2,
                                    enum TokenKind kind,
                                    const struct Type *ref_type)
{
	struct Expr *ptr_expr1 = calloc(1, sizeof(struct Expr));
	struct Expr *ptr_expr2 = calloc(1, sizeof(struct Expr));
	*ptr_expr1 = *ref_expr;
	*ptr_expr2 = *ref_expr2;

	struct Expr new_expr;
	new_expr.details.type = *ref_type;
	new_expr.details.true_type = *ref_type;
	new_expr.category = SIMPLE_BINARY_EXPR;
	new_expr.simple_binary_operator = to_simplebinop(kind);
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct Expr comma_op(const struct Expr *ref_expr,
                            const struct Expr *ref_expr2,
                            const struct Type *ref_type)
{
	struct Expr *ptr_expr1 = calloc(1, sizeof(struct Expr));
	struct Expr *ptr_expr2 = calloc(1, sizeof(struct Expr));
	*ptr_expr1 = *ref_expr;
	*ptr_expr2 = *ref_expr2;

	struct Expr new_expr;
	new_expr.details.type = *ref_type;
	new_expr.details.true_type = *ref_type;
	new_expr.category = COMMA_EXPR;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;

	return new_expr;
}

static struct Expr pointer_plusorminus_int(const struct AnalyzerState *ptr_ps,
                                           const struct Expr *ref_expr,
                                           const struct Expr *ref_expr2,
                                           enum TokenKind kind)
{
	struct Expr *ptr_expr1 = calloc(1, sizeof(struct Expr));
	struct Expr *ptr_expr2 = calloc(1, sizeof(struct Expr));
	*ptr_expr1 = *ref_expr;
	*ptr_expr2 = *ref_expr2;

	struct Expr new_expr;
	new_expr.details = ref_expr->details;
	new_expr.category = kind == OP_PLUS ? POINTER_PLUS_INT : POINTER_MINUS_INT;
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = ptr_expr2;
	new_expr.ptr3 = 0;
	const struct Type deref = deref_type(&ref_expr->details.type);
	new_expr.size_info_for_pointer_arith = size_of(ptr_ps, &deref);

	return new_expr;
}

static int is_local_var(const struct ScopeChain *ref_t, const char *str)
{
	if (isElem(ref_t->var_table, str)) {
		return 1;
	} else if (ref_t->outer == 0) {
		/* most outer, but cannot be found */
		return 0;
	} else {
		return is_local_var(ref_t->outer, str);
	}
}

static struct LocalVarInfo resolve_name_locally(const struct ScopeChain *ref_t,
                                                const char *str)
{
	if (isElem(ref_t->var_table, str)) {
		struct LocalVarInfo *ptr_varinfo = lookup(ref_t->var_table, str);
		return *ptr_varinfo;
	} else if (ref_t->outer == 0) {
		/* most outer, but cannot be found */
		fprintf(stderr, "%s is not declared locally\n", str);
		exit(EXIT_FAILURE);
	} else {
		return resolve_name_locally(ref_t->outer, str);
	}
}

int isAssign(enum TokenKind opkind)
{
	return (opkind == OP_EQ || opkind == OP_PLUS_EQ || opkind == OP_MINUS_EQ ||
	        opkind == OP_ASTERISK_EQ || opkind == OP_SLASH_EQ ||
	        opkind == OP_PERCENT_EQ || opkind == OP_LSHIFT_EQ ||
	        opkind == OP_RSHIFT_EQ || opkind == OP_AND_EQ ||
	        opkind == OP_HAT_EQ || opkind == OP_OR_EQ);
}

static struct Type resolve_name_globally(struct Map2 /*<Type>*/ *m,
                                         const char *str)
{
	if (isElem(m, str)) {
		struct Type *ptr_type = lookup(m, str);
		return *ptr_type;
	} else {
		fprintf(stderr, "%s is not declared globally\n", str);
		exit(EXIT_FAILURE);
	}
}

static const struct EnumeratorAndValue *
get_global_enumerator(const struct Vector /*<EnumeratorAndValue>*/ *ref_list,
                      const char *name)
{
	for (int i = 0; i < ref_list->length; i++) {
		const struct EnumeratorAndValue *ptr_vec_i = ref_list->vector[i];
		if (strcmp(ptr_vec_i->ident, name) == 0) {
			return ptr_vec_i;
		}
	}
	return 0;
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
			assert0("cannot happen" && 0);
	}
}

static struct Expr unary_op_(const struct Expr *ref_expr, enum TokenKind kind,
                             const struct Type *ref_type)
{
	struct Expr *ptr_expr1 = calloc(1, sizeof(struct Expr));
	*ptr_expr1 = *ref_expr;

	struct Expr new_expr;
	new_expr.details.type = *ref_type;
	new_expr.category = UNARY_OP_EXPR;
	new_expr.unary_operator = to_unaryop(kind);
	new_expr.ptr1 = ptr_expr1;
	new_expr.ptr2 = 0;
	new_expr.ptr3 = 0;

	return new_expr;
}

static enum SimpleBinOp op_before_assign(enum TokenKind kind)
{
	switch (kind) {
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
			assert0(
			    "op_before_assign called before a non-assignment operator" &&
			    0);
	}
}

static void cast_to_null_pointer_if_possible(struct Expr *ref_e,
                                             const struct TypePair *ref_details)
{
	if (ref_e->category == INT_VALUE && ref_e->int_value == 0) {
		ref_e->category = NULLPTR;
		ref_e->details = *ref_details;
	}
}

struct Expr typecheck_binary_expression(const struct AnalyzerState *ptr_ps,
                                        const struct Expr *ref_expr,
                                        const struct Expr *ref_expr2,
                                        enum TokenKind op);

struct Expr typecheck_expression(struct AnalyzerState *ptr_ps,
                                 const struct UntypedExpr *ref_uexpr)
{
	const struct UntypedExpr uexpr = *ref_uexpr;
	switch (uexpr.category) {
		case DOT_EXPR: {
			struct Expr struct_expr = typecheck_expression(ptr_ps, uexpr.ptr1);
			const char *ident_after_dot = uexpr.ident_after_dot;
			if (struct_expr.details.type.type_category != STRUCT_) {
				fprintf(stderr, "member is requested but the left operand is "
				                "not a struct\n");
				exit(EXIT_FAILURE);
			}

			const char *tag = struct_expr.details.type.s.struct_tag;
			const struct StructInternalCompleteInfo *ptr_info =
			    lookup(ptr_ps->global_struct_tag_map, tag);
			if (!ptr_info) {
				fprintf(
				    stderr,
				    "tried to use a member of incomplete type `struct %s`\n",
				    tag);
				exit(EXIT_FAILURE);
			}

			struct Vector /* <TypeAndIdent> */ vec =
			    *ptr_info->info.ptr_types_and_idents;
			int nth_member = -1;

			struct Expr expr;
			for (int i = 0; i < vec.length; i++) {
				const struct TypeAndIdent *ptr_vec_i = vec.vector[i];
				if (strcmp(ptr_vec_i->ident_str, ident_after_dot) == 0) {
					nth_member = i;

					struct Type t2 = ptr_vec_i->type;
					if_array_convert_to_ptr_(&t2);
					expr.details.type = t2;
					expr.details.true_type = ptr_vec_i->type;
					break;
				}
			}

			if (nth_member == -1) {
				fprintf(stderr, "member `%s` does not belong to struct `%s`\n",
				        ident_after_dot, tag);
				exit(EXIT_FAILURE);
			}

			int offset = ptr_info->offset_vec[nth_member];

			struct Expr *ptr_struct_expr = calloc(1, sizeof(struct Expr));
			*ptr_struct_expr = struct_expr;

			expr.category = STRUCT_AND_OFFSET;
			expr.struct_offset = offset;
			expr.ptr1 = ptr_struct_expr;
			expr.ptr2 = 0;
			expr.ptr3 = 0;
			return expr;
		}
		case SIZEOF_TYPE: {
			struct Expr expr;
			expr.details.type = INT_TYPE();
			expr.int_value =
			    size_of(ptr_ps, &uexpr.operand_of_sizeof_or_alignof);
			expr.category = INT_VALUE;
			return expr;
		}
		case ALIGNOF_TYPE: {
			struct Expr expr;
			expr.details.type = INT_TYPE();
			expr.int_value =
			    align_of(ptr_ps, &uexpr.operand_of_sizeof_or_alignof);
			expr.category = INT_VALUE;
			return expr;
		}
		case UNARY_EXPR: {
			switch (uexpr.operator_) {
				case OP_NOT: {
					enum TokenKind kind = uexpr.operator_;
					struct Expr expr = typecheck_expression(ptr_ps, uexpr.ptr1);
					expect_scalar(&expr.details.type, "operand of logical not");

					struct Type t = INT_TYPE();
					struct Expr new_expr = unary_op_(&expr, kind, &t);

					return new_expr;
				}
				case OP_TILDA:
				case OP_PLUS:
				case OP_MINUS: {
					enum TokenKind kind = uexpr.operator_;
					struct Expr expr = typecheck_expression(ptr_ps, uexpr.ptr1);
					expect_integral(
					    &expr.details.type,
					    "operand of bitnot, unary plus or unary minus");

					/* integral promotion */
					struct Type t = INT_TYPE();
					struct Expr new_expr = unary_op_(&expr, kind, &t);

					return new_expr;
				}

				case OP_PLUS_PLUS:
				case OP_MINUS_MINUS: {
					enum TokenKind opkind = uexpr.operator_;

					struct Expr expr = typecheck_expression(ptr_ps, uexpr.ptr1);

					struct Expr new_expr =
					    unary_op_(&expr, opkind, &expr.details.type);
					if (expr.details.type.type_category == PTR_) {
						const struct Type deref =
						    deref_type(&expr.details.type);
						new_expr.size_info_for_pointer_arith =
						    size_of(ptr_ps, &deref);
					}
					return new_expr;
				}

				case OP_AND: {
					const struct Expr expr =
					    typecheck_expression(ptr_ps, uexpr.ptr1);

					struct Type type = expr.details.type;

					if (type.type_category == PTR_ &&
					    expr.details.true_type.type_category == ARRAY) {
						type = expr.details.true_type;
					}

					const struct Type ptr_to_type_ = ptr_to_type(&type);
					struct Expr new_expr =
					    unary_op_(&expr, OP_AND, &ptr_to_type_);

					return new_expr;
				}
				case OP_ASTERISK: {
					const struct Expr expr =
					    typecheck_expression(ptr_ps, uexpr.ptr1);

					const struct Type type = deref_type(&expr.details.type);

					struct Type t2 = type;
					if_array_convert_to_ptr_(&t2);

					struct Expr new_expr = unary_op_(&expr, OP_ASTERISK, &t2);
					new_expr.details.true_type = type;

					return new_expr;
				}
				default: {
					fprintf(stderr,
					        "FAILURE::::::: INVALID TOKEN %d in unary\n",
					        uexpr.operator_);
					exit(EXIT_FAILURE);
				}
			}
		}
		case FUNCCALL: {
			const char *ident_str = uexpr.var_name;

			struct Type ret_type;
			if (!isElem(ptr_ps->func_info_map, ident_str)) {
				fprintf(stderr, "Undeclared function `%s()` detected.\n",
				        ident_str);
				fprintf(stderr, "Assumes that `%s()` returns `int`\n",
				        ident_str);
				ret_type = INT_TYPE();
			} else {
				struct Type *ptr_func_info =
				    lookup(ptr_ps->func_info_map, ident_str);
				ret_type = *(ptr_func_info->derived_from);
			}

			struct Expr expr;
			expr.args = init_vector();

			if (ret_type.type_category == STRUCT_) {
				expr.size_info_for_struct_assign = size_of(ptr_ps, &ret_type);
				char *str = calloc(20, sizeof(char));
				sprintf(str, "@anon%d", -ptr_ps->newest_offset);

				int offset = add_local_var_to_scope(ptr_ps, &ret_type, str);

				enum SystemVAbiClass abi_class =
				    system_v_abi_class_of(ptr_ps, &ret_type);

				expr.local_var_offset = offset;

				if (abi_class == INTEGER_CLASS) {
					expr.category = FUNCCALL_EXPR_RETURNING_INTEGER_CLASS;
				} else {
					expr.category = FUNCCALL_EXPR_RETURNING_MEMORY_CLASS;

					struct Expr *ptr_arg = calloc(1, sizeof(struct Expr));

					/* push pointer to anon as the first argument */
					{
						struct Expr anon_var_expr;
						anon_var_expr.details.type = ret_type;
						anon_var_expr.details.true_type = ret_type;
						anon_var_expr.category = LOCAL_VAR_;
						anon_var_expr.local_var_offset = offset;

						const struct Type ptr_to_type_ = ptr_to_type(&ret_type);
						*ptr_arg =
						    unary_op_(&anon_var_expr, OP_AND, &ptr_to_type_);
					}
					push_vector(&expr.args, ptr_arg);
				}
			} else {
				expr.category = FUNCCALL_EXPR;
			}

			for (int counter = 0; counter < uexpr.arg_exprs_vec.length;
			     counter++) {
				const struct UntypedExpr *ptr =
				    uexpr.arg_exprs_vec.vector[counter];

				struct Expr *ptr_arg = calloc(1, sizeof(struct Expr));
				*ptr_arg = typecheck_expression(ptr_ps, ptr);
				push_vector(&expr.args, ptr_arg);
				if (counter > 5) {
					unsupported("calling with 7 or more arguments");
				}
			}

			expr.details.type = ret_type;
			expr.global_var_name = ident_str;
			return expr;
		}
		case POSTFIX_EXPR: {
			struct Expr expr = typecheck_expression(ptr_ps, uexpr.ptr1);
			struct Expr *ptr_expr1 = calloc(1, sizeof(struct Expr));
			*ptr_expr1 = expr;

			enum TokenKind opkind = uexpr.operator_;

			struct Expr new_expr;
			new_expr.details.type = expr.details.type;
			new_expr.details.true_type = expr.details.type;
			new_expr.category =
			    opkind == OP_PLUS_PLUS ? POSTFIX_INCREMENT : POSTFIX_DECREMENT;
			new_expr.ptr1 = ptr_expr1;
			new_expr.ptr2 = 0;
			new_expr.ptr3 = 0;
			if (expr.details.type.type_category == PTR_) {
				const struct Type deref = deref_type(&expr.details.type);
				new_expr.size_info_for_pointer_arith = size_of(ptr_ps, &deref);
			}

			return new_expr;
		}
		case INT_LITERAL_: {
			struct Expr expr;
			expr.details.type = INT_TYPE();
			expr.int_value = uexpr.int_value;
			expr.category = INT_VALUE;
			return expr;
		}
		case VAR: {
			const char *name = uexpr.var_name;

			if (!is_local_var(&ptr_ps->scope_chain, name)) {
				const struct EnumeratorAndValue *ptr_enum_and_value =
				    get_global_enumerator(&ptr_ps->global_enumerator_list,
				                          name);
				if (ptr_enum_and_value) {
					struct Expr expr;
					expr.details.type = INT_TYPE();
					expr.details.true_type = INT_TYPE();
					expr.category = INT_VALUE;
					expr.int_value = ptr_enum_and_value->value;
					return expr;
				}
				struct Type type =
				    resolve_name_globally(ptr_ps->global_vars_type_map, name);

				struct Expr expr;
				struct Type t2 = type;
				if_array_convert_to_ptr_(&t2);

				expr.details.type = t2;
				expr.details.true_type = type;
				expr.category = GLOBAL_VAR_;
				expr.global_var_name = name;
				return expr;
			} else {
				struct LocalVarInfo info =
				    resolve_name_locally(&ptr_ps->scope_chain, name);

				struct Expr expr;
				struct Type t2 = info.type;
				if_array_convert_to_ptr_(&t2);

				expr.details.type = t2;
				expr.details.true_type = info.type;
				expr.category = LOCAL_VAR_;
				expr.local_var_offset = info.offset;
				return expr;
			}
		}
		case STRING_LITERAL_: {
			const struct Type char_type = CHAR_TYPE();

			struct Expr expr;
			expr.details.type = ptr_to_type(&char_type);
			expr.details.true_type =
			    arr_of_type(&char_type, strlen(uexpr.literal_string) + 1);
			expr.category = STRING_LITERAL;
			expr.literal_string = uexpr.literal_string;

			return expr;
		}
		case CONDITIONAL: {
			struct Expr expr = typecheck_expression(ptr_ps, uexpr.ptr1);
			struct Expr true_branch = typecheck_expression(ptr_ps, uexpr.ptr2);
			struct Expr false_branch = typecheck_expression(ptr_ps, uexpr.ptr3);
			expect_type(
			    ptr_ps, &false_branch.details.type, &true_branch.details.type,
			    "mismatch of type in the false branch and the true branch");
			struct Expr *ptr_expr1 = calloc(1, sizeof(struct Expr));
			struct Expr *ptr_expr2 = calloc(1, sizeof(struct Expr));
			struct Expr *ptr_expr3 = calloc(1, sizeof(struct Expr));
			*ptr_expr1 = expr;
			*ptr_expr2 = true_branch;
			*ptr_expr3 = false_branch;

			struct Expr new_expr;
			new_expr.details = true_branch.details;
			new_expr.category = CONDITIONAL_EXPR;
			new_expr.ptr1 = ptr_expr1;
			new_expr.ptr2 = ptr_expr2;
			new_expr.ptr3 = ptr_expr3;
			return new_expr;
		}
		case BINARY_EXPR: {
			const struct Expr expr = typecheck_expression(ptr_ps, uexpr.ptr1);
			const struct Expr expr2 = typecheck_expression(ptr_ps, uexpr.ptr2);
			return typecheck_binary_expression(ptr_ps, &expr, &expr2,
			                                   uexpr.operator_);
		}
	}
	assert("should not pass here" && 0);
}

struct Expr typecheck_binary_expression(const struct AnalyzerState *ptr_ps,
                                        const struct Expr *ref_expr,
                                        const struct Expr *ref_expr2,
                                        enum TokenKind op)
{
	const struct Expr expr = *ref_expr;
	const struct Expr expr2 = *ref_expr2;
	{
		{
			if (isAssign(op)) {
				if (expr.details.type.type_category == ARRAY ||
				    (expr.details.type.type_category == PTR_ &&
				     expr.details.true_type.type_category == ARRAY)) {
					fprintf(stderr, "array is not an lvalue\n");
					exit(EXIT_FAILURE);
				}

				struct Expr new_expr;
				new_expr.details = expr.details;
				struct Expr *ptr_expr1 = calloc(1, sizeof(struct Expr));
				struct Expr *ptr_expr2 = calloc(1, sizeof(struct Expr));
				*ptr_expr1 = expr;
				new_expr.ptr1 = ptr_expr1;
				new_expr.ptr3 = 0;

				if (op == OP_EQ) {
					struct Expr expr2_new = expr2;

					if (expr.details.type.type_category == PTR_) {
						cast_to_null_pointer_if_possible(&expr2_new,
						                                 &expr.details);
					}

					expect_type(ptr_ps, &expr.details.type,
					            &expr2_new.details.type,
					            "mismatch in assignment operator");

					*ptr_expr2 = expr2_new;
					new_expr.ptr2 = ptr_expr2;

					if (expr.details.type.type_category == STRUCT_) {
						new_expr.category = STRUCT_ASSIGNMENT_EXPR;
						new_expr.size_info_for_struct_assign =
						    size_of(ptr_ps, &expr.details.type);
					} else {
						new_expr.category = ASSIGNMENT_EXPR;
					}
					return new_expr;
				}

				/* op != OP_EQ */
				if (expr.details.type.type_category == STRUCT_) {
					fprintf(stderr, "invalid compound assignment operator "
					                "used on a struct\n");
					exit(EXIT_FAILURE);
				}

				if (expr.details.type.type_category == PTR_ &&
				    (op == OP_PLUS_EQ || op == OP_MINUS_EQ)) {

					expect_integral(&expr2.details.type,
					                "right side of += or -= to a pointer");

					*ptr_expr2 = expr2;

					new_expr.category = COMPOUND_ASSIGNMENT_EXPR;
					new_expr.simple_binary_operator = op_before_assign(op);

					new_expr.ptr2 = ptr_expr2;

					const struct Type deref = deref_type(&expr.details.type);
					new_expr.size_info_for_pointer_arith =
					    size_of(ptr_ps, &deref);

					return new_expr;
				}

				if (expr.details.type.type_category == PTR_) {
					fprintf(stderr, "invalid compound assignment operator "
					                "used on a pointer\n");
					exit(EXIT_FAILURE);
				}

				expect_type(ptr_ps, &expr.details.type, &expr2.details.type,
				            "mismatch in assignment operator");

				*ptr_expr2 = expr2;

				new_expr.category = COMPOUND_ASSIGNMENT_EXPR;
				new_expr.simple_binary_operator = op_before_assign(op);

				new_expr.ptr2 = ptr_expr2;

				if (expr.details.type.type_category == PTR_ &&
				    (op == OP_PLUS_EQ || op == OP_MINUS_EQ)) {
					const struct Type deref = deref_type(&expr.details.type);
					new_expr.size_info_for_pointer_arith =
					    size_of(ptr_ps, &deref);
				}

				return new_expr;
			}

			switch (op) {
				case OP_PLUS: {
					const struct Type type1 = expr.details.type;
					const struct Type type2 = expr2.details.type;

					if (is_integral(&type1)) {
						if (is_integral(&type2)) {
							const struct Type t = INT_TYPE();
							return simple_binary_op(&expr, &expr2, OP_PLUS, &t);
						} else if (type2.type_category == PTR_) {
							/* swapped */
							return pointer_plusorminus_int(ptr_ps, &expr2,
							                               &expr, OP_PLUS);
						} else {
							fprintf(stderr, "invalid type `");
							debug_print_type(&type2);
							fprintf(stderr,
							        "`as the right operand of binary +\n");
							exit(EXIT_FAILURE);
						}
					} else if (type1.type_category == PTR_) {
						expect_integral(
						    &expr2.details.type,
						    "cannot add a pointer/struct to a pointer");
						return pointer_plusorminus_int(ptr_ps, &expr, &expr2,
						                               OP_PLUS);
					} else {
						fprintf(stderr, "invalid type `");
						debug_print_type(&type1);
						fprintf(stderr, "`as the left operand of binary +\n");
						exit(EXIT_FAILURE);
					}
				}
				case OP_MINUS: {

					struct Type type1 = expr.details.type;
					struct Type type2 = expr2.details.type;

					if (is_integral(&type1)) {
						if (is_integral(&type2)) {
							const struct Type t = INT_TYPE();
							return simple_binary_op(&expr, &expr2, OP_MINUS,
							                        &t);
						} else if (type2.type_category == PTR_) {

							fprintf(stderr, "cannot subtract a pointer "
							                "from an integer.\n");
							exit(EXIT_FAILURE);
						}

					} else if (type1.type_category == PTR_) {
						if (is_integral(&type2)) {

							/* pointer minus int */
							return pointer_plusorminus_int(ptr_ps, &expr,
							                               &expr2, OP_MINUS);
						} else {
							/* pointer minus pointer */

							struct Expr *ptr_expr1 =
							    calloc(1, sizeof(struct Expr));
							struct Expr *ptr_expr2 =
							    calloc(1, sizeof(struct Expr));
							*ptr_expr1 = expr;
							*ptr_expr2 = expr2;

							struct Expr new_expr;
							new_expr.details.type = INT_TYPE();
							new_expr.details.true_type = INT_TYPE();
							new_expr.category = POINTER_MINUS_POINTER;
							new_expr.ptr1 = ptr_expr1;
							new_expr.ptr2 = ptr_expr2;
							new_expr.ptr3 = 0;
							const struct Type deref =
							    deref_type(&expr.details.type);
							new_expr.size_info_for_pointer_arith =
							    size_of(ptr_ps, &deref);

							return new_expr;
						}
					} else {
						fprintf(stderr, "invalid type `");
						debug_print_type(&type1);
						fprintf(stderr, "`as the left operand of binary -\n");
						exit(EXIT_FAILURE);
					}
				}
				case OP_AND_AND: {
					expect_scalar(&expr.details.type, "operand of logical AND");
					expect_scalar(&expr2.details.type,
					              "operand of logical AND");
					const struct Type t = INT_TYPE();
					return binary_op(&expr, &expr2, LOGICAL_AND_EXPR, &t);
				}
				case OP_OR_OR: {
					expect_scalar(&expr.details.type, "operand of logical OR");
					expect_scalar(&expr2.details.type, "operand of logical OR");
					const struct Type t = INT_TYPE();
					return binary_op(&expr, &expr2, LOGICAL_OR_EXPR, &t);
				}
				case OP_OR:
				case OP_AND:
				case OP_LSHIFT:
				case OP_RSHIFT:
				case OP_HAT:
				case OP_ASTERISK:
				case OP_SLASH:
				case OP_PERCENT: {
					expect_integral(&expr.details.type,
					                "left operand of an operator");
					expect_integral(&expr2.details.type,
					                "right operand of an operator");
					return simple_binary_op(&expr, &expr2, op,
					                        &expr2.details.type);
				}
				case OP_GT:
				case OP_GT_EQ:
				case OP_LT:
				case OP_LT_EQ:
				case OP_NOT_EQ:
				case OP_EQ_EQ: {
					struct Expr expr_new = expr;
					struct Expr expr2_new = expr2;

					if (expr.details.type.type_category == PTR_) {
						cast_to_null_pointer_if_possible(&expr2_new,
						                                 &expr.details);
					} else if (expr2.details.type.type_category == PTR_) {

						cast_to_null_pointer_if_possible(&expr_new,
						                                 &expr2.details);
					}

					expect_type(ptr_ps, &expr_new.details.type,
					            &expr2_new.details.type,
					            "mismatch in operands of an "
					            "equality/comparison operator");
					const struct Type t = INT_TYPE();
					return simple_binary_op(&expr_new, &expr2_new, op, &t);
				}
				case OP_COMMA: {
					if (expr2.details.type.type_category == STRUCT_) {
						unsupported(
						    "struct as the right operand of comma operator");
					}
					return comma_op(&expr, &expr2, &expr2.details.type);
				}
				default: {
					fprintf(stderr,
					        "FAILURE::::::: INVALID TOKEN %d in binary expr\n",
					        op);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
}
