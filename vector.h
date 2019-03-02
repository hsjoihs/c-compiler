struct Vector {
	int length;
	int _allocated_length;
	const void **vector;
};
struct Vector *init_vector_(void);
void extend_vector(struct Vector *ptr);
void push_vector(struct Vector *ptr, const void *tok);
const void *pop_vector(struct Vector *ptr);
struct Vector init_vector(void);
void concat_vector(struct Vector *ptr_ans, const struct Vector *vec, const char *msg);
