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
void gen_for_part4(int label1, int label2);
void gen_push_address_of_local(int offset);
void gen_write_to_local_8byte(int offset);
void gen_push_from_local_8byte(int offset);
void gen_peek_and_dereference(void);
void gen_peek_and_dereference_8byte(void);
void gen_assign_to_backed_up_address(void);
void gen_jump(int label, const char *comment);
void gen_pop2nd_to_local_8byte(int offset);
void gen_discard2nd_8byte(void);
void gen_pop_to_reg_8byte(const char *str);
void gen_write_register_to_local_8byte(const char *str, int offset);
void gen_epilogue_8byte(int label);
void gen_push_ret_of_8byte(const char *fname);
void gen_cltq(void);
void gen_op_8byte(const char *str);
void gen_mul_by_const(int mul);
void gen_swap(void);
void gen_div_by_const(int num);
void gen_before_epilogue(int label1, int label2, int capacity);
void gen_after_prologue(int label1, int label2);
void gen_write_to_global_8byte(const char *ident);

#define GARBAGE_INT ((int)0xCCCCCCCC)
#define BOOL int

enum TokenKind {
	OP_PLUS = 1,
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
	LEFT_BRACKET,
	RIGHT_BRACKET,
};

struct Token{
	enum TokenKind kind;
	int int_value;
	const char* ident_str;
};

struct ParserState;

void print_token(struct Token tok);
struct Token get_token(const char** ptr_to_str);

struct ExprInfo parseprint_expression(struct ParserState *ptr_ps,
                      const struct Token **ptr_tokvec);
struct ExprInfo parseprint_assignment_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
struct ExprInfo parseprint_primary_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
struct ExprInfo parseprint_inclusive_OR_expression(struct ParserState *ptr_ps,
                                   const struct Token **ptr_tokvec);
struct ExprInfo parseprint_cast_expression(struct ParserState *ptr_ps,
                           const struct Token **ptr_tokvec);
struct ExprInfo parseprint_unary_expression(struct ParserState *ptr_ps,
                            const struct Token **ptr_tokvec);
struct ExprInfo parseprint_postfix_expression(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
struct ExprInfo parseprint_conditional_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec);
void parseprint_compound_statement(struct ParserState *ptr_ps,
                              const struct Token **ptr_tokvec);
struct ExprInfo parseprint_logical_OR_expression(struct ParserState *ptr_ps,
                                 const struct Token **ptr_tokvec);
struct ExprInfo parseprint_logical_AND_expression(struct ParserState *ptr_ps,
                                  const struct Token **ptr_tokvec);

void print_inc_or_dec(struct ParserState *ptr_ps, const char *name,
                enum TokenKind opkind);

void print_binary_op(enum TokenKind kind);
void print_unary_prefix_op(enum TokenKind kind);

struct ParamInfo;

struct ParamInfos {
	struct ParamInfo **param_vec; 
	/*
	 points to the array of (ParamInfo*). 
	 terminated by NULL. 
	 if the param_vec itself is NULL, that means there is no info.
	 */
};



enum typ_ { INT_ = 1, PTR_, ARRAY, FN };

struct Type {
	enum typ_ type_category;
	struct Type *derived_from;
	int array_length;
	struct ParamInfos param_infos;
};

struct ParamInfo {
	struct Type param_type;
	const char *ident_str;
};

int size_of(struct Type type);


enum expr_info_ {
	LOCAL_VAR = 1,
	DEREFERENCED_ADDRESS,
	NOT_ASSIGNABLE
};

struct ExprInfo {
	enum expr_info_ info;
	struct Type type;
	int offset;
};

extern struct ExprInfo FIXME;
extern struct Type INT_TYPE;

void expect_type(struct ExprInfo expr_info, struct Type expected_type, int id);
struct ExprInfo remove_leftiness(struct ExprInfo info);

struct ExprInfo UNASSIGNABLE(struct Type type);

struct Type deref_type(struct Type t);
int is_equal(struct Type t1, struct Type t2);
int is_pointer(struct Type t);
struct Type ptr_of_type_to_ptr_to_type(struct Type* ptr_type);

struct Type parse_var_declarator(const struct Token **ptr_tokvec,
                                 const char **ptr_to_ident_str);
void expect_and_consume(const struct Token **ptr_tokvec, enum TokenKind kind,
                        const char *str);
void error_unexpected_token(const struct Token *tokvec, const char *str);

struct Token *read_all_tokens_(const char *str);
int count_all_tokens(const char *str);
void debug_print_type(struct Type type);


struct FuncInfo {
	struct Type ret_type;
	struct ParamInfos param_infos;
};

int can_start_a_type(const struct Token *tokvec);

struct Type parse_declarator(const struct Token **ptr_tokvec,
                                 const char **ptr_to_ident_str);
