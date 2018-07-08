struct vector_T init_vector_T(int initial_length)
{
	struct vector_T res;
	res.length = initial_length;
	res._allocated_length = (initial_length > 128 ? initial_length : 128) * 2;
	res.vector = calloc(res._allocated_length, sizeof(struct T));
	return res;
}

void extend_vector_T(struct vector_T* ptr)
{
	if (ptr->_allocated_length < ptr->length + 1) {
		ptr->vector = realloc(ptr->vector, ptr->_allocated_length * 2 * sizeof(struct T));
		ptr->_allocated_length *= 2;
	}
}

