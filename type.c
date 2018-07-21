#include "header.h"
#include "map.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int size_of(struct Type type)
{
	switch (type.type) {
		case INT_:
			return 4;
		case PTR_:
			return 8;
	}
}

struct Type INT_TYPE(void) {
	struct Type i;
	i.type = INT_;
	i.pointer_of = 0;
	return i;
} 

int is_equal(struct Type t1, struct Type t2)
{
	if (t1.type == INT_ && t2.type == INT_) {
		return 1;
	}

	if (t1.type == PTR_ && t2.type == PTR_) {
		return is_equal(*t1.pointer_of, *t2.pointer_of);
	}

	return 0;
}

void debug_print_type(struct Type type)
{
	switch (type.type) {
		case PTR_:
			debug_print_type(*type.pointer_of);
			fprintf(stderr, "*");
			return;
		case INT_:
			fprintf(stderr, "int");
			return;
	}
}

void push_type(struct TypeCheckerState *ptr_tcs, struct Type type)
{
	struct vector type_stack = ptr_tcs->type_stack;
	struct Type *p_type = calloc(1, sizeof(struct Type));
	*p_type = type;
	push_vector(&type_stack, p_type);
	ptr_tcs->type_stack = type_stack;
}

struct Type *pop_type(struct TypeCheckerState *ptr_tcs)
{
	struct vector type_stack = ptr_tcs->type_stack;
	struct Type *p_top_type = pop_vector(&type_stack);
	return p_top_type;
}

void pop_type_and_expect(struct TypeCheckerState *ptr_tcs,
                         struct Type expected_type)
{
	struct Type *p_popped_type = pop_type(ptr_tcs);
	if (!is_equal(*p_popped_type, expected_type)) {
		fprintf(stderr, "type mismatch: expected `");
		debug_print_type(expected_type);
		fprintf(stderr, "` but got `");
		debug_print_type(*p_popped_type);
		fprintf(stderr, "`.\n");
		exit(EXIT_FAILURE);
	}
}

void typecheck_push_int(struct TypeCheckerState *ptr_tcs, int num)
{
	push_type(ptr_tcs, INT_TYPE());
	gen_push_int(num);
}
