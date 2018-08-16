#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_strictly_equal(struct Type t1, struct Type t2);

struct Type INT_TYPE = {INT_};
struct Type CHAR_TYPE = {CHAR_};

int size_of(struct Type type)
{
	switch (type.type_category) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case CHAR_:
			return 1;
		case ARRAY:
			return type.array_length * size_of(*type.derived_from);
		case FN:
			fprintf(stderr, "function type does not have size\n");
			exit(EXIT_FAILURE);
	}
}

int align_of(struct Type type)
{
	switch (type.type_category) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case CHAR_:
			return 1;
		case ARRAY:
			return align_of(*type.derived_from);
		case FN:
			fprintf(stderr, "function type does not have size or alignment\n");
			exit(EXIT_FAILURE);
	}
}

int is_strictly_equal(struct Type t1, struct Type t2)
{
	if (t1.type_category == INT_ && t2.type_category == INT_) {
		return 1;
	}

	if (t1.type_category == CHAR_ && t2.type_category == CHAR_) {
		return 1;
	}

	if (t1.type_category == PTR_ && t2.type_category == PTR_) {
		return is_strictly_equal(*t1.derived_from, *t2.derived_from);
	}

	if (t1.type_category == ARRAY && t2.type_category == ARRAY) {
		return is_strictly_equal(*t1.derived_from, *t2.derived_from) &&
		       (t1.array_length == t2.array_length);
	}

	return 0;
}

int is_compatible(struct Type t1, struct Type t2)
{
	if (is_strictly_equal(t1, t2)) {
		return 1;
	}

	if (t1.type_category == INT_ && t2.type_category == CHAR_) {
		return 1;
	}

	if (t1.type_category == CHAR_ && t2.type_category == INT_) {
		return 1;
	}

	if (t1.type_category == PTR_ && t2.type_category == ARRAY) {
		return is_strictly_equal(*t1.derived_from, *t2.derived_from);
	}

	if (t1.type_category == ARRAY && t2.type_category == PTR_) {
		return is_strictly_equal(*t1.derived_from, *t2.derived_from) &&
		       (t1.array_length == t2.array_length);
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
		case CHAR_:
			fprintf(stderr, "char");
			return;
		case ARRAY:
			fprintf(stderr, "array (length %d) of ", type.array_length);
			debug_print_type(*type.derived_from);
			return;
		case FN:
			fprintf(stderr, "function (");
			if (!type.param_infos.vector) {
				fprintf(stderr, "param: no info");
			} else {
				if (type.param_infos.length < 2) {
					const struct ParamInfo *vec_0 =
					    (const struct ParamInfo *)type.param_infos.vector[0];
					fprintf(stderr, "%s: ", vec_0->ident_str);
					debug_print_type(vec_0->param_type);
				} else {
					fprintf(stderr, "params: \n");
					for (int i = 0; i < type.param_infos.length; i++) {
						const struct ParamInfo *ptr_paraminfo =
						    (const struct ParamInfo *)
						        type.param_infos.vector[i];
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

void expect_type(struct Type actual_type, struct Type expected_type,
                 const char *message)
{

	if (!is_compatible(actual_type, expected_type)) {
		fprintf(stderr, "Unmatched type: expected `");
		debug_print_type(expected_type);
		fprintf(stderr, "`, but got `");
		debug_print_type(actual_type);
		fprintf(stderr, "`.\n");
		fprintf(stderr, "context: %s\n", message);
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

int is_array(struct Type t) { return t.type_category == ARRAY; }

struct Type if_array_convert_to_ptr(struct Type t)
{
	if (is_array(t)) {
		t.type_category = PTR_;
	}
	return t;
}

struct Type ptr_of_type_to_ptr_to_type(struct Type *ptr_type)
{
	struct Type type;
	type.type_category = PTR_;
	type.derived_from = ptr_type;
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
typedef struct Type TypeNode; /* the link list is incomplete and should not be
                                 brought outside */

static struct Type from_type3_to_type(const void **type3)
{
	struct Type type;
	TypeNode elem = *(const TypeNode *)type3[0];
	type.type_category = elem.type_category;
	switch (elem.type_category) {
		case INT_:
			return type;

		case CHAR_:
			return type;

		case PTR_:
		case ARRAY:
		case FN: {
			type.array_length = elem.array_length;
			/* garbage enters here in case of PTR_ and FN */
			type.param_infos = elem.param_infos;
			/* garbage enters here in case of PTR_ and ARRAY */

			struct Type *ptr_to_current_type = calloc(1, sizeof(struct Type));
			*ptr_to_current_type = from_type3_to_type(type3 + 1);

			type.derived_from = ptr_to_current_type;
			return type;
		}
	}
}

struct ParamInfo *parse_param(const struct Token **ptr_tokvec)
{
	const char *ident_str;
	struct ParamInfo *ptr_param_info = calloc(1, sizeof(struct ParamInfo));

	struct Type type = parse_declarator(ptr_tokvec, &ident_str);
	if (type.type_category == FN) {
		/* shall be adjusted to `pointer to func`, according to the spec */
		struct Type *ptr_type = calloc(1, sizeof(struct Type));
		*ptr_type = type;
		ptr_param_info->param_type = ptr_of_type_to_ptr_to_type(ptr_type);
	} else {
		/* convert to pointer */
		if (type.type_category == ARRAY) {
			type.type_category = PTR_;
		}
		ptr_param_info->param_type = type;
	}
	ptr_param_info->ident_str = ident_str;
	return ptr_param_info;
}

struct Type *parse_type_specifier(const struct Token **ptr_tokvec)
{
	const struct Token *tokvec = *ptr_tokvec;

	enum TokenKind tok = tokvec[0].kind;
	TypeNode *ptr = calloc(1, sizeof(TypeNode));

	if (tok == RES_CHAR) {
		ptr->type_category = CHAR_;
		++tokvec;
	} else if (tok == RES_INT) {
		ptr->type_category = INT_;
		++tokvec;
	} else {
		error_unexpected_token(tokvec, "type name `int` or `char`");
	}

	*ptr_tokvec = tokvec;
	return ptr;
}

/* `int a`, `int *a` */
struct Type parse_declarator(const struct Token **ptr_tokvec,
                             const char **ptr_to_ident_str)
{
	const struct Token *tokvec = *ptr_tokvec;

	struct Type *ptr_base_type = parse_type_specifier(&tokvec);

	struct Vector vec = init_vector();

	int asterisk_num = 0;
	for (; tokvec[0].kind == OP_ASTERISK; ++tokvec) {
		asterisk_num++;
	}

	if (tokvec[0].kind == IDENT_OR_RESERVED) {
		*ptr_to_ident_str = tokvec[0].ident_str;
		++tokvec;
	} else {
		error_unexpected_token(tokvec, "an identifier in the declarator");
	}

	while (1) {
		if (tokvec[0].kind == LEFT_BRACKET) {
			++tokvec;
			expect_and_consume(&tokvec, LIT_DEC_INTEGER,
			                   "an integer while parsing a declaration");
			int length = tokvec[-1].int_value;
			expect_and_consume(&tokvec, RIGHT_BRACKET,
			                   "closing ] while parsing a declaration");

			TypeNode a;
			a.type_category = ARRAY;
			a.array_length = length;
			TypeNode *ptr = calloc(1, sizeof(TypeNode));
			*ptr = a;
			push_vector(&vec, ptr);
			continue;
		}
		if (tokvec[0].kind != LEFT_PAREN) {
			break;
		}

		++tokvec;
		if (tokvec[0].kind == RIGHT_PAREN) {
			++tokvec;
			TypeNode f;
			f.type_category = FN;
			f.param_infos.vector = (const void **)0; /* crucial */
			TypeNode *ptr = calloc(1, sizeof(TypeNode));
			*ptr = f;
			push_vector(&vec, ptr);
		} else if (can_start_a_type(tokvec)) {
			TypeNode f;
			f.type_category = FN;
			f.param_infos.vector = calloc(100, sizeof(struct ParamInfo *));

			f.param_infos.vector[0] = parse_param(&tokvec);

			int i = 1;

			while (1) {
				enum TokenKind kind = tokvec[0].kind;
				if (kind != OP_COMMA) {
					break;
				}
				++tokvec;

				f.param_infos.vector[i] = parse_param(&tokvec);
				i++;
			}

			f.param_infos.length = i;

			TypeNode *ptr = calloc(1, sizeof(TypeNode));
			*ptr = f;
			push_vector(&vec, ptr);

			expect_and_consume(&tokvec, RIGHT_PAREN,
			                   "closing ) while parsing functional type");
		}
	}

	while (asterisk_num-- > 0) {
		TypeNode *ptr = calloc(1, sizeof(TypeNode));
		ptr->type_category = PTR_;
		push_vector(&vec, ptr);
	}

	*ptr_tokvec = tokvec;

	push_vector(&vec, ptr_base_type);

	return from_type3_to_type(vec.vector);
}

int can_start_a_type(const struct Token *tokvec)
{
	return tokvec[0].kind == RES_INT || tokvec[0].kind == RES_CHAR ||
	       tokvec[0].kind == RES_STRUCT;
}
