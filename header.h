#pragma once


void print_prologue(int alloc_size, const char* fname);
void push_int(int num);
void op_ints(const char* str);
void compare_ints(const char* str);
void mul_ints(void);
void div_ints(void);
void rem_ints(void);
void shift_ints(const char* str);
void unary_not(void);
void unary(const char* str);
void push_from_local(int offset);
void write_to_local(int offset);
void return_with_label(int label);
void print_epilogue(int label, int alloc_size);
void push_eax(void);
void push_ret_of(const char* fname);
void pop_to_reg(const char *str);
void write_register_to_local(const char* str, int offset);
void logical_AND_set(int offset, int label1, int label2);
void logical_OR_set(int offset, int label1, int label2);
void logical_AND_final(int final_depth, int label1, int label2);
void logical_OR_final(int final_depth, int label1, int label2);


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
	OP_OR,
	OP_EQ_EQ,
	OP_NOT_EQ,
	OP_NOT,
	OP_TILDA,
	OP_EQ,
	IDENT_OR_RESERVED,
	RES_RETURN,
	SEMICOLON,
	OP_HAT,
	QUESTION,
	COLON,
	LEFT_BRACE,
	RIGHT_BRACE,
	OP_AND_AND,
	OP_OR_OR
};

struct Token{
	enum TokenKind kind;
	int int_value;
	const char* ident_str;
};

void print_token(struct Token tok);
struct Token get_token(const char** ptr_to_str);

struct charptANDint {
	const char* ptr;
	int value;
};

struct Int {
	int i;
};
