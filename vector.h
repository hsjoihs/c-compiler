struct Vector {
	int length;
	int _allocated_length;
	const void **vector;
};
struct Vector init_vector();
void extend_vector(struct Vector *ptr);
void push_vector(struct Vector *ptr, const void *tok);
const void *pop_vector(struct Vector *ptr);
