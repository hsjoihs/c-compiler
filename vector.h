/* AUTOMATICALLY GENERATED. DO NOT EDIT. */
#pragma once 
#include "header.h"
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
struct vector_Int {
	int length;
	int _allocated_length;
	struct Int* vector;
};
struct vector_Int init_vector_Int(int initial_length);
void extend_vector_Int(struct vector_Int* ptr);
void push_vector_Int(struct vector_Int* ptr, struct Int tok);
struct Int pop_vector_Int(struct vector_Int* ptr);
