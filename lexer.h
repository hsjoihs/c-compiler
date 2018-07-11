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
	OP_PERCENT
};

struct Token{
	enum TokenKind kind;
	int int_value;
};

void print_token(struct Token tok);
struct Token get_token(const char** ptr_to_str);
