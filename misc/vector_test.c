struct Vector {
	int length;
	int _allocated_length;
	void **vector;
};
struct Vector *init_vector_();
void extend_vector();
void push_vector();
void *pop_vector();
struct Vector init_vector();

void *calloc();
void *realloc();

struct Vector *init_vector_(void)
{
	struct Vector *ptr_res;
	ptr_res = calloc(1, sizeof(struct Vector));
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

void push_vector(struct Vector *ptr, void *tok)
{
	extend_vector(ptr);
	ptr->vector[ptr->length] = tok;
	++(ptr->length);
}

void *pop_vector(struct Vector *ptr)
{
	if (ptr->length == 0) {
		/*assert("tried to pop an empty vector of type `void*`" && 0);*/
	}
	--(ptr->length);
	return ptr->vector[ptr->length]; /* safe, since it is not yet released or
	                                    anything */
}
