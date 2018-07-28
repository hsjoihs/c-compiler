#include "header.h"
#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int size_of(struct Type type)
{
	switch (type.type_domain) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
		case ARRAY:
			return type.array_length * size_of(*type.pointer_of);
		default:
			fprintf(stderr, "Unknown type with id %d\n", type.type_domain);
			exit(EXIT_FAILURE);
	}
}

int is_equal(struct Type t1, struct Type t2)
{
	if (t1.type_domain == INT_ && t2.type_domain == INT_) {
		return 1;
	}

	if (t1.type_domain == PTR_ && t2.type_domain == PTR_) {
		return is_equal(*t1.pointer_of, *t2.pointer_of);
	}

	return 0;
}

void debug_print_type(struct Type type)
{
	switch (type.type_domain) {
		case PTR_:
			fprintf(stderr, "pointer of ");
			debug_print_type(*type.pointer_of);
			return;
		case INT_:
			fprintf(stderr, "int");
			return;
		case ARRAY:
			fprintf(stderr, "array (length %d) of ", type.array_length);
			debug_print_type(*type.pointer_of);
			return;
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
	switch (t.type_domain) {
		case PTR_:
			return *t.pointer_of;

		default:
			fprintf(stderr, "Unmatched type: expected a pointer, but got `");
			debug_print_type(t);
			fprintf(stderr, "`.\n");
			exit(EXIT_FAILURE);
	}
}

int is_pointer(struct Type t) { return t.type_domain == PTR_; }

struct Type ptr_of_type_to_ptr_to_type(struct Type *ptr_type)
{
	struct Type type;
	type.type_domain = PTR_;
	type.pointer_of = ptr_type;
	type.array_length = GARBAGE_INT;
	return type;
}
