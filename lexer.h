#pragma once

#define GARBAGE_INT 0xCCCCCCCC
#define BOOL int

enum TokenKind {
	OP_PLUS,
	OP_MINUS,
	END,
	LIT_DEC_INTEGER,
	OP_ASTERISK,
	LEFT_PAREN,
	RIGHT_PAREN,
	OP_SLASH,
	OP_PERCENT,
	OP_COMMA,
	OP_LSHIFT,
	OP_LT_EQ,
	OP_LT,
	OP_RSHIFT,
	OP_GT_EQ,
	OP_GT,
	OP_AND,
	EMPTY
};

struct Token{
	enum TokenKind kind;
	int int_value;
};

void print_token(struct Token tok);
struct Token get_token(const char** ptr_to_str);
