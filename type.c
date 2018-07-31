#include "header.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Type INT_TYPE = {INT_, 0, GARBAGE_INT, {(struct ParamInfo **)0}};

int size_of(struct Type type)
{
	switch (type.type_category) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case ARRAY:
			return type.array_length * size_of(*type.derived_from);
		case FN:
			fprintf(stderr, "function type does not have size\n");
			exit(EXIT_FAILURE);
	}
}

int is_equal(struct Type t1, struct Type t2)
{
	if (t1.type_category == INT_ && t2.type_category == INT_) {
		return 1;
	}

	if (t1.type_category == PTR_ && t2.type_category == PTR_) {
		return is_equal(*t1.derived_from, *t2.derived_from);
	}

	return 0;
}

void debug_print_type(struct Type type)
{
	switch (type.type_category) {
		case PTR_:
			fprintf(stderr, "pointer to ");
			debug_print_type(*type.derived_from);
			return;
		case INT_:
			fprintf(stderr, "int");
			return;
		case ARRAY:
			fprintf(stderr, "array (length %d) of ", type.array_length);
			debug_print_type(*type.derived_from);
			return;
		case FN:
			fprintf(stderr, "function (");
			if (!type.param_infos.param_vec) {
				fprintf(stderr, "param: no info");
			} else {
				struct ParamInfo **vec = type.param_infos.param_vec;
				if (vec[0] && !vec[1]) {
					fprintf(stderr, "%s: ", vec[0]->ident_str);
					debug_print_type(vec[0]->param_type);
				} else {
					fprintf(stderr, "params: \n");
					for (int i = 0; vec[i]; i++) {
						struct ParamInfo *ptr_paraminfo = vec[i];
						fprintf(stderr, "  %s: ", ptr_paraminfo->ident_str);
						debug_print_type(ptr_paraminfo->param_type);
						fprintf(stderr, "\n");
					}
				}
			}
			fprintf(stderr, ") returning ");
			debug_print_type(*type.derived_from);
	}
}

void expect_type(struct ExprInfo expr_info, struct Type expected_type, int id)
{

	if (!is_equal(expr_info.type, expected_type)) {
		fprintf(stderr, "Unmatched type: expected `");
		debug_print_type(expected_type);
		fprintf(stderr, "`, but got `");
		debug_print_type(expr_info.type);
		fprintf(stderr, "`.\n");
		fprintf(stderr, "Debug info: my typecheck # is %d\n", id);
		exit(EXIT_FAILURE);
	}
}

struct Type deref_type(struct Type t)
{
	switch (t.type_category) {
		case PTR_:
			return *t.derived_from;

		default:
			fprintf(stderr, "Unmatched type: expected a pointer, but got `");
			debug_print_type(t);
			fprintf(stderr, "`.\n");
			exit(EXIT_FAILURE);
	}
}

int is_pointer(struct Type t) { return t.type_category == PTR_; }

struct Type ptr_of_type_to_ptr_to_type(struct Type *ptr_type)
{
	struct Type type;
	type.type_category = PTR_;
	type.derived_from = ptr_type;
	type.array_length = GARBAGE_INT;
	return type;
}

struct Type ptr_of_type_to_arr_of_type(struct Type *ptr_type, int length)
{
	struct Type type;
	type.type_category = ARRAY;
	type.derived_from = ptr_type;
	type.array_length = length;
	return type;
}

/***************************************
 * pure parsers with respect to types. *
 ***************************************/

enum t2 {
	POINTER_TO,
	ARRAY_OF,
	FUNCTION_RETURNING,
	INT_TYPE_,
};

struct type3_elem {
	enum t2 type_category;
	int array_length;
	struct ParamInfos param_infos;
};

struct Type3 {
	int length;
	int _allocated_length;
	struct type3_elem *vector;
	const char *ident_str;
};

void push_to_type3(struct Type3 *ptr, struct type3_elem tok)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector = realloc(ptr->vector, ptr->_allocated_length * 2 *
		                                       sizeof(struct type3_elem));
		ptr->_allocated_length *= 2;
	}
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

struct Type from_type3_to_type(const struct type3_elem *type3)
{
	switch (type3[0].type_category) {
		case INT_TYPE_:
			return INT_TYPE;
		case POINTER_TO: {
			++type3;
			struct Type *ptr_to_current_type = calloc(1, sizeof(struct Type));
			*ptr_to_current_type = from_type3_to_type(type3);
			return ptr_of_type_to_ptr_to_type(ptr_to_current_type);
		}
		case ARRAY_OF: {
			++type3;
			struct Type *ptr_to_current_type = calloc(1, sizeof(struct Type));
			*ptr_to_current_type = from_type3_to_type(type3);
			return ptr_of_type_to_arr_of_type(ptr_to_current_type,
			                                  type3[-1].array_length);
		}
		case FUNCTION_RETURNING: {
			++type3;
			struct Type *ptr_to_current_type = calloc(1, sizeof(struct Type));
			*ptr_to_current_type = from_type3_to_type(type3);
			struct ParamInfos param_infos = type3[-1].param_infos;

			struct Type type;
			type.type_category = FN;
			type.derived_from = ptr_to_current_type;
			type.array_length = GARBAGE_INT;
			type.param_infos = param_infos;
			return type;
		}
	}
}

void parse_dcl(const struct Token **ptr_tokvec, struct Type3 *ptr_type3);

void parse_param(const struct Token **ptr_tokvec, struct type3_elem *ptr_f,
                 int i)
{
	const char *ident_str;
	struct ParamInfo *ptr_param_info = calloc(1, sizeof(struct ParamInfo));

	struct Type type = parse_var_declarator(ptr_tokvec, &ident_str);
	if (type.type_category == FN) {
		/* shall be adjusted to `pointer to func`, according to the spec */
		struct Type *ptr_type = calloc(1, sizeof(struct Type));
		*ptr_type = type;
		ptr_param_info->param_type = ptr_of_type_to_ptr_to_type(ptr_type);
	} else {
		/* convert to pointer */
		if (type.type_category == ARRAY) {
			type.type_category = PTR_;
			type.array_length = GARBAGE_INT;
		}
		ptr_param_info->param_type = type;
	}
	ptr_param_info->ident_str = ident_str;
	ptr_f->param_infos.param_vec[i] = ptr_param_info;
}

void parse_dirdcl(const struct Token **ptr_tokvec, struct Type3 *ptr_type3)
{
	const struct Token *tokvec = *ptr_tokvec;

	if (tokvec[0].kind == LEFT_PAREN) {
		++tokvec;
		parse_dcl(&tokvec, ptr_type3);
		expect_and_consume(&tokvec, RIGHT_PAREN,
		                   "closing ) while parsing a declaration");
	} else if (tokvec[0].kind == IDENT_OR_RESERVED) {
		ptr_type3->ident_str = tokvec[0].ident_str;
		++tokvec;
	} else {
		error_unexpected_token(tokvec, "( or an identifier in the declarator");
		exit(EXIT_FAILURE); /* silence the warning */
	}

	while (1) {
		if (tokvec[0].kind == LEFT_BRACKET) {
			++tokvec;
			expect_and_consume(&tokvec, LIT_DEC_INTEGER,
			                   "an integer while parsing a declaration");
			int length = tokvec[-1].int_value;
			expect_and_consume(&tokvec, RIGHT_BRACKET,
			                   "closing ] while parsing a declaration");

			struct type3_elem a;
			a.type_category = ARRAY_OF;
			a.array_length = length;
			push_to_type3(ptr_type3, a);
		} else if (tokvec[0].kind == LEFT_PAREN) {
			++tokvec;
			if (tokvec[0].kind == RIGHT_PAREN) {
				++tokvec;
				struct type3_elem f;
				f.type_category = FUNCTION_RETURNING;
				f.param_infos.param_vec = (struct ParamInfo **)0;
				push_to_type3(ptr_type3, f);
			} else if (can_start_a_type(tokvec)) {
				struct type3_elem f;
				f.type_category = FUNCTION_RETURNING;
				f.param_infos.param_vec =
				    calloc(100, sizeof(struct ParamInfo *));

				parse_param(&tokvec, &f, 0);

				int i = 1;

				while (1) {
					enum TokenKind kind = tokvec[0].kind;
					if (kind != OP_COMMA) {
						break;
					}
					++tokvec;

					parse_param(&tokvec, &f, i);
					i++;
				}

				f.param_infos.param_vec[i] = (struct ParamInfo *)0;

				push_to_type3(ptr_type3, f);

				expect_and_consume(&tokvec, RIGHT_PAREN,
				                   "closing ) while parsing functional type");
			}
		} else {
			break;
		}
	}
	*ptr_tokvec = tokvec;
}

void parse_dcl(const struct Token **ptr_tokvec, struct Type3 *ptr_type3)
{
	const struct Token *tokvec = *ptr_tokvec;

	int ns = 0;
	for (; tokvec[0].kind == OP_ASTERISK; ++tokvec) {
		ns++;
	}

	parse_dirdcl(&tokvec, ptr_type3);

	while (ns-- > 0) {
		struct type3_elem p = {
		    POINTER_TO, GARBAGE_INT, {(struct ParamInfo **)0}};
		push_to_type3(ptr_type3, p);
	}

	*ptr_tokvec = tokvec;
}

/* `int a`, `int *a` */
struct Type parse_declarator(const struct Token **ptr_tokvec,
                             const char **ptr_to_ident_str)
{
	expect_and_consume(ptr_tokvec, RES_INT, "type name `int`");

	struct Type3 type3;
	type3.length = 0;
	type3._allocated_length = 30;
	type3.vector = calloc(30, sizeof(struct type3_elem));
	type3.ident_str = NULL;

	parse_dcl(ptr_tokvec, &type3);

	*ptr_to_ident_str = type3.ident_str;

	struct type3_elem i = {INT_TYPE_, GARBAGE_INT, {(struct ParamInfo **)0}};
	push_to_type3(&type3, i);

	struct Type type = from_type3_to_type(type3.vector);

	return type;
}

struct Type parse_var_declarator(const struct Token **ptr_tokvec,
                                 const char **ptr_to_ident_str)
{
	struct Type type = parse_declarator(ptr_tokvec, ptr_to_ident_str);
	if (type.type_category == FN) {
		fprintf(stderr,
		        "Expected a non-function type, but got function type\n");
		exit(EXIT_FAILURE);
	} else {
		return type;
	}
}

int can_start_a_type(const struct Token *tokvec)
{
	return tokvec[0].kind == RES_INT;
}
