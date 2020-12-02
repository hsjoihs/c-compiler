struct Vector {
	int length;
	int capacity;
	void **vector;
};
struct Vector *init_vector_(); void extend_vector();
void push_vector(); void *pop_vector(); struct Vector init_vector();
void *calloc(); void *realloc();

struct Vector *init_vector_(void)
{
	struct Vector *ptr_res;
	ptr_res = calloc(1, sizeof(struct Vector));
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
void push_vector(struct Vector *ptr, void *tok) {
	extend_vector(ptr); ptr->vector[(ptr->length)++] = tok;
}
void *pop_vector(struct Vector *ptr) {return ptr->vector[--(ptr->length)]; }
