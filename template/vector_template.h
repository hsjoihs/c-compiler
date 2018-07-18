struct vector_T {
	int length;
	int _allocated_length;
	U* vector;
};
struct vector_T init_vector_T(int initial_length);
void extend_vector_T(struct vector_T* ptr);
void push_vector_T(struct vector_T* ptr, U tok);
U pop_vector_T(struct vector_T* ptr);
