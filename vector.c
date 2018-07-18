/* AUTOMATICALLY GENERATED. DO NOT EDIT. */
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
struct vector_Token init_vector_Token(int initial_length)
{
	struct vector_Token res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(struct Token));
	return res;
}

void extend_vector_Token(struct vector_Token *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector = realloc(ptr->vector, ptr->_allocated_length * 2 *
		                                       sizeof(struct Token));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type Token");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}

void push_vector_Token(struct vector_Token *ptr, struct Token tok)
{
	extend_vector_Token(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

struct Token pop_vector_Token(struct vector_Token *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `struct Token`" && 0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
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
struct vector_Int init_vector_Int(int initial_length)
{
	struct vector_Int res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(struct Int));
	return res;
}

void extend_vector_Int(struct vector_Int *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector = realloc(ptr->vector,
		                      ptr->_allocated_length * 2 * sizeof(struct Int));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type Int");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}

void push_vector_Int(struct vector_Int *ptr, struct Int tok)
{
	extend_vector_Int(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

struct Int pop_vector_Int(struct vector_Int *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `struct Int`" && 0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
struct vector_int init_vector_int(int initial_length)
{
	struct vector_int res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(int));
	return res;
}

void extend_vector_int(struct vector_int *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector =
		    realloc(ptr->vector, ptr->_allocated_length * 2 * sizeof(int));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type int");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}

void push_vector_int(struct vector_int *ptr, int tok)
{
	extend_vector_int(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

int pop_vector_int(struct vector_int *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `int`" && 0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
struct vector_voidptr init_vector_voidptr(int initial_length)
{
	struct vector_voidptr res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(void *));
	return res;
}

void extend_vector_voidptr(struct vector_voidptr *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector =
		    realloc(ptr->vector, ptr->_allocated_length * 2 * sizeof(void *));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type voidptr");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}

void push_vector_voidptr(struct vector_voidptr *ptr, void *tok)
{
	extend_vector_voidptr(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

void *pop_vector_voidptr(struct vector_voidptr *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `void*`" && 0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
