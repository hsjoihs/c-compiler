#pragma once
#include "map.h"


void gen_prologue(int alloc_size, const char* fname);
void gen_push_int(int num);
void gen_op_ints(const char* str);
void gen_compare_ints(const char* str);
void gen_mul_ints(void);
void gen_div_ints(void);
void gen_rem_ints(void);
void gen_shift_ints(const char* str);
void gen_unary_not(void);
void gen_unary(const char* str);
void gen_push_from_local(int offset);
void gen_write_to_local(int offset);
void gen_return_with_label(int label);
void gen_epilogue(int label);
void gen_push_ret_of(const char* fname);
void gen_pop_to_reg(const char *str);
void gen_write_register_to_local(const char* str, int offset);
void gen_logical_AND_set(int offset, int label1, int label2);
void gen_logical_OR_set(int offset, int label1, int label2);
void gen_logical_AND_final(int final_depth, int label1, int label2);
void gen_logical_OR_final(int final_depth, int label1, int label2);
void gen_ternary_part1(int label1, int label2);
void gen_ternary_part2(int label1, int label2);
void gen_ternary_part3(int label1, int label2);
void gen_if_else_part1(int label1, int label2);
void gen_if_else_part2(int label1, int label2);
void gen_if_else_part3(int label1, int label2);
void gen_label(int label1);
void gen_do_while_final(int label1, int label2);
void gen_while_part2(int label1, int label2);
void gen_discard(void);
void gen_for_part4(int label1, int label2, int label3);
void gen_push_address_of_local(int offset);
void gen_write_to_local_8byte(int offset);
void gen_push_from_local_8byte(int offset);
void gen_peek_and_dereference(void);
void gen_peek_and_dereference_8byte(void);

#define GARBAGE_INT ((int)0xCCCCCCCC)
#define BOOL int

enum TokenKind {
	OP_PLUS,
	OP_MINUS,
	END,
	BEGINNING,
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
	OP_OR_OR,
	OP_PLUS_EQ,
	OP_MINUS_EQ,
	OP_ASTERISK_EQ,
	OP_SLASH_EQ,
	OP_PERCENT_EQ,
	OP_LSHIFT_EQ,
	OP_RSHIFT_EQ,
	OP_AND_EQ,
	OP_HAT_EQ,
	OP_OR_EQ,
	RES_IF,
	RES_ELSE,
	RES_DO,
	RES_WHILE,
	RES_BREAK,
	RES_CONTINUE,
	OP_PLUS_PLUS,
	OP_MINUS_MINUS,
	RES_FOR,
	RES_INT,
};

struct Token{
	enum TokenKind kind;
	int int_value;
	const char* ident_str;
};

struct ParserState;

void print_token(struct Token tok);
struct Token get_token(const char** ptr_to_str);

void parse_expression(struct ParserState *ptr_ps,
                      const struct Token **ptr_tokvec);
void parse_assignment_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
void parse_primary_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
void parse_inclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
void parse_cast_expression(struct ParserState *ptr_ps,
                           const struct Token **ptr_tokvec);
void parse_unary_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec);
void parse_postfix_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
void parse_conditional_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec);
void parse_compound_statement(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
void parse_logical_OR_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
void parse_logical_AND_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec);

void inc_or_dec(struct ParserState *ptr_ps, const char *name,
                enum TokenKind opkind);

void binary_op(enum TokenKind kind);
void print_unary_prefix_op(enum TokenKind kind);

enum typ_ { INT_, PTR_ };

struct Type {
	enum typ_ type;
	struct Type *pointer_of;
};
int size_of(struct Type type);


struct VarInfo {
	struct Type type;
	int offset;
};

struct VarTableList {
	struct map var_table;
	struct VarTableList *outer;
};

