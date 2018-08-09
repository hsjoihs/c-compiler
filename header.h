#include "map.h"

#define GARBAGE_INT ((int)0xCCCCCCCC)

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
	RES_CHAR,
	LIT_STRING
};

struct Token {
	enum TokenKind kind;
	int int_value;
	const char *ident_str;
	const char *literal_str;
};

struct ParserState;

void print_token(struct Token tok);
struct Token get_token(const char **ptr_to_str);

struct ParamInfo;

struct ParamInfos {
	struct ParamInfo **param_vec;
	/*
	 points to the array of (ParamInfo*).
	 terminated by NULL.
	 if the param_vec itself is NULL, that means there is no info.
	 */
};

enum typ_ { INT_ = 1, PTR_, ARRAY, FN, CHAR_ };

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
	NOT_ASSIGNABLE,
	GLOBAL_VAR
};

struct ExprInfo {
	enum expr_info_ info;
	struct Type type;
	struct Type true_type;
	int offset;
};

extern struct Type INT_TYPE;

void expect_type(struct ExprInfo expr_info, struct Type expected_type, int id);
struct ExprInfo remove_leftiness(struct ExprInfo info);

struct ExprInfo UNASSIGNABLE(struct Type type);

struct Type deref_type(struct Type t);
int is_compatible(struct Type t1, struct Type t2);
int is_pointer(struct Type t);
struct Type ptr_of_type_to_ptr_to_type(struct Type *ptr_type);

struct Type parse_var_declarator(const struct Token **ptr_tokvec,
                                 const char **ptr_to_ident_str);
void expect_and_consume(const struct Token **ptr_tokvec, enum TokenKind kind,
                        const char *str);
_Noreturn void error_unexpected_token(const struct Token *tokvec,
                                      const char *str);

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

_Noreturn void unimplemented(const char *str);

enum expr_category {
	SIMPLE_BINARY_EXPR = 2,
	POINTER_PLUS_INT,
	POINTER_MINUS_INT,
	POINTER_MINUS_POINTER,
	CONDITIONAL_EXPR,
	UNARY_OP_EXPR,
	LOCAL_VAR_,
	GLOBAL_VAR_,
	INT_VALUE,
	POSTFIX_INCREMENT,
	POSTFIX_DECREMENT,
	FUNCCALL_EXPR,
	ASSIGNMENT_EXPR,
	LOGICAL_OR_EXPR,
	LOGICAL_AND_EXPR,
	STRING_LITERAL
};

enum SimpleBinOp {
	SIMPLE_BIN_OP_PLUS,
	SIMPLE_BIN_OP_MINUS,
	SIMPLE_BIN_OP_ASTERISK,
	SIMPLE_BIN_OP_SLASH,
	SIMPLE_BIN_OP_PERCENT,
	SIMPLE_BIN_OP_COMMA,
	SIMPLE_BIN_OP_LT,
	SIMPLE_BIN_OP_LT_EQ,
	SIMPLE_BIN_OP_LSHIFT,
	SIMPLE_BIN_OP_GT,
	SIMPLE_BIN_OP_GT_EQ,
	SIMPLE_BIN_OP_RSHIFT,
	SIMPLE_BIN_OP_AND,
	SIMPLE_BIN_OP_OR,
	SIMPLE_BIN_OP_EQ_EQ,
	SIMPLE_BIN_OP_NOT_EQ,
	SIMPLE_BIN_OP_HAT
};

enum UnaryOp {
	UNARY_OP_NOT,
	UNARY_OP_TILDA,
	UNARY_OP_PLUS,
	UNARY_OP_MINUS,

	UNARY_OP_PLUS_PLUS,
	UNARY_OP_MINUS_MINUS,

	UNARY_OP_AND,

	UNARY_OP_ASTERISK
};

struct Expression {
	struct ExprInfo details;
	enum expr_category category;
	enum SimpleBinOp simple_binary_operator;
	enum UnaryOp unary_operator;
	struct Expression *ptr1;
	struct Expression *ptr2;
	struct Expression *ptr3;
	int int_value;
	const char *global_var_name;
	const char *literal_string;
	struct Expression *arg_expr_vec;
	int arg_length;
};

struct Expression parse_cast_expression(struct ParserState *ptr_ps,
                                        const struct Token **ptr_tokvec);
struct Expression parse_logical_OR_expression(struct ParserState *ptr_ps,
                                              const struct Token **ptr_tokvec);

struct Expression remove_leftiness_(struct Expression expr);
struct Expression binary_op_(struct Expression expr, struct Expression expr2,
                             enum expr_category cat, struct ExprInfo exprinfo);

enum SimpleBinOp to_simplebinop(enum TokenKind t);
struct Expression simple_binary_op(struct Expression expr,
                                   struct Expression expr2, enum TokenKind kind,
                                   struct ExprInfo exprinfo);

void print_simple_binary_op(enum SimpleBinOp kind);

enum UnaryOp to_unaryop(enum TokenKind t);

struct Expression combine_by_add_or_sub(struct Expression expr,
                                        struct Expression expr2,
                                        enum TokenKind kind);

int is_array(struct Type t);
struct Type if_array_convert_to_ptr(struct Type t);
enum SimpleBinOp op_before_assign(enum TokenKind kind);

void read_all_tokens_debug(const char *str);
struct Expression combine_by_add_or_sub(struct Expression expr,
                                        struct Expression expr2,
                                        enum TokenKind kind);

struct Type ptr_of_type_to_arr_of_type(struct Type *ptr_type, int length);

extern struct Type CHAR_TYPE;
