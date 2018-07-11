/* AUTOMATICALLY GENERATED. DO NOT EDIT. */
#include "lexer.h"

struct charptANDint {
	const char* ptr;
	int value;
};
struct vector_Token {
	int length;
	int _allocated_length;
	struct Token* vector;
};
struct vector_Token init_vector_Token(int initial_length);
void extend_vector_Token(struct vector_Token* ptr);
void push_vector_Token(struct vector_Token* ptr, struct Token tok);
struct Token pop_vector_Token(struct vector_Token* ptr);
struct vector_charptANDint {
	int length;
	int _allocated_length;
	struct charptANDint* vector;
};
struct vector_charptANDint init_vector_charptANDint(int initial_length);
void extend_vector_charptANDint(struct vector_charptANDint* ptr);
void push_vector_charptANDint(struct vector_charptANDint* ptr, struct charptANDint tok);
struct charptANDint pop_vector_charptANDint(struct vector_charptANDint* ptr);
