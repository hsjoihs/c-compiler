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
		case STRUCT_:
			unsupported("size of struct");
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
		case STRUCT_:
			unsupported("width of struct");
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
		case STRUCT_:
			fprintf(stderr, "struct %s", type.struct_tag);
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
