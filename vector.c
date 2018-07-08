/* AUTOMATICALLY GENERATED. DO NOT EDIT. */
#include <stdlib.h>
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
		ptr->_allocated_length *= 2;
	}
}

void push_vector_Token(struct vector_Token* ptr, struct Token tok)
{
	extend_vector_Token(ptr);
	ptr->vector[ptr->length] = tok;
	ptr->length++; 
}
