#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct vector init_vector(int initial_length)
{
	struct vector res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(void *));
	return res;
}

void extend_vector(struct vector *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector =
		    realloc(ptr->vector, ptr->_allocated_length * 2 * sizeof(void *));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type void*");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}

void push_vector(struct vector *ptr, void *tok)
{
	extend_vector(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

void *pop_vector(struct vector *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `void*`" && 0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
