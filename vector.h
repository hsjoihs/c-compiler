/* AUTOMATICALLY GENERATED. DO NOT EDIT. */
#include "lexer.h"
struct vector_Token {
	int length;
	int _allocated_length;
	struct Token* vector;
};
struct vector_Token init_vector_Token(int initial_length);
void extend_vector_Token(struct vector_Token* ptr);
