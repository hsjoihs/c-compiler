/* AUTOMATICALLY GENERATED. DO NOT EDIT. */
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "vector.h"
struct vector_Token init_vector_Token(int initial_length)
{
	struct vector_Token res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(struct Token));
	return res;
}

void extend_vector_Token(struct vector_Token* ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector = realloc(ptr->vector, ptr->_allocated_length * 2 * sizeof(struct Token));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty vector of type Token");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}

void push_vector_Token(struct vector_Token* ptr, struct Token tok)
{
	extend_vector_Token(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length); 
}

struct Token pop_vector_Token(struct vector_Token* ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `struct Token`" && 0);
	}
	--(ptr->length); 
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or anything */
}
