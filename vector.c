#include "vector.h"
#include <assert.h>
#include <stdlib.h>

struct Vector *init_vector_(void)
{
	struct Vector *ptr_res = calloc(1, sizeof(struct Vector));
	ptr_res->length = 0;
	ptr_res->_allocated_length = 256;
	ptr_res->vector = calloc(ptr_res->_allocated_length, sizeof(void *));
	return ptr_res;
}

void extend_vector(struct Vector *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {
		ptr->vector =
		    realloc(ptr->vector, ptr->_allocated_length * 2 * sizeof(void *));

		ptr->_allocated_length *= 2;
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
		assert("tried to pop an empty vector of type `void*`" && 0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
