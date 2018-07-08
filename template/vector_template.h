struct vector_T {
	int length;
	int _allocated_length;
	struct T* vector;
};
struct vector_T init_vector_T(int initial_length);
void extend_vector_T(struct vector_T* ptr);
void push_vector_T(struct vector_T* ptr, struct T tok);
