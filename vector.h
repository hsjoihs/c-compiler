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

struct vector_int {
	int length;
	int _allocated_length;
	int* vector;
};
struct vector_int init_vector_int(int initial_length);
void extend_vector_int(struct vector_int* ptr);
void push_vector_int(struct vector_int* ptr, int tok);
int pop_vector_int(struct vector_int* ptr);

struct vector_voidptr {
	int length;
	int _allocated_length;
	void** vector;
};
struct vector_voidptr init_vector_voidptr(int initial_length);
void extend_vector_voidptr(struct vector_voidptr* ptr);
void push_vector_voidptr(struct vector_voidptr* ptr, void* tok);
void* pop_vector_voidptr(struct vector_voidptr* ptr);

