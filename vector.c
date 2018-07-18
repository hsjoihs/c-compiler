/* AUTOMATICALLY GENERATED. DO NOT EDIT. */
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct vector_charptANDint init_vector_charptANDint(int initial_length)
{
	struct vector_charptANDint res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(struct charptANDint));
	return res;
}

void extend_vector_charptANDint(struct vector_charptANDint *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector = realloc(ptr->vector, ptr->_allocated_length * 2 *
		                                       sizeof(struct charptANDint));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type charptANDint");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}

void push_vector_charptANDint(struct vector_charptANDint *ptr,
                              struct charptANDint tok)
{
	extend_vector_charptANDint(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

struct charptANDint pop_vector_charptANDint(struct vector_charptANDint *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `struct charptANDint`" &&
		       0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
