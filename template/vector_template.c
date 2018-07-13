struct vector_T init_vector_T(int initial_length)
{
	struct vector_T res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(struct T));
	return res;
}

void extend_vector_T(struct vector_T *ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {

		ptr->vector =
		    realloc(ptr->vector, ptr->_allocated_length * 2 * sizeof(struct T));

		if (!(ptr->vector)) { /* fails when the memory runs out */
			fprintf(stderr, "memory ran out when trying to reallocate an empty "
			                "vector of type T");
			abort();
		}

		ptr->_allocated_length *= 2;
	}
}

void push_vector_T(struct vector_T *ptr, struct T tok)
{
	extend_vector_T(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

struct T pop_vector_T(struct vector_T *ptr)
{
	if (ptr->length == 0) {
		assert("tried to pop an empty vector of type `struct T`" && 0);
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
