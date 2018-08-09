struct vector {
	int length;
	int _allocated_length;
	void **vector;
};
struct vector init_vector();
void extend_vector(struct vector *ptr);
void push_vector(struct vector *ptr, void *tok);
void *pop_vector(struct vector *ptr);
