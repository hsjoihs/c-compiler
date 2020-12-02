#include "vector.h"
#include "std.h"
#include "std_io.h"

struct Vector init_vector(void) { return *init_vector_(); }

struct Vector *init_vector_(void)
{
	struct Vector *ptr_res = calloc(1, sizeof(struct Vector));
	ptr_res->length = 0;
	ptr_res->capacity = 256;
	ptr_res->vector = calloc(ptr_res->capacity, sizeof(void *));
	return ptr_res;
}

void extend_vector(struct Vector *ptr)
{
	if (ptr->capacity < ptr->length + 1) {
		ptr->vector =
		    realloc(ptr->vector, ptr->capacity * 2 * sizeof(void *));

		ptr->capacity *= 2;
	}
}

void push_vector(struct Vector *ptr, const void *tok)
{
	extend_vector(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

const void *pop_vector(struct Vector *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector" && 0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}

void concat_vector(struct Vector *ptr_ans, const struct Vector *ptr_vec)
{
	if (!ptr_ans) {
		fprintf(stderr, "NULL POINTER IN THE FIRST ARGUMENT\n");
		exit(1);
	}

	if (!ptr_vec) {
		fprintf(stderr, "NULL POINTER IN THE SECOND ARGUMENT\n");
		exit(1);
	}

	if (!ptr_vec->vector) {
		fprintf(stderr, "OH MY GOD: INVALID VECTOR IN THE SECOND ARGUMENT\n");
		exit(1);
	}

	for (int i = 0; i < ptr_vec->length; i++) {
		push_vector(ptr_ans, ptr_vec->vector[i]);
	}
}
