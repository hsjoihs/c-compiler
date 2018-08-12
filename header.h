#include "map.h"
#include "vector.h"

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

void print_token(struct Token tok);
struct Token get_token(const char **ptr_to_str);

struct ParamInfo;

struct ParamInfos {
	int length;
	struct ParamInfo **param_vec;
	/*
	 points to the array of (ParamInfo*).
	 terminated by NULL.
	 if the param_vec itself is NULL, that means there is no info.
	 */
};

enum TypeCategory { INT_ = 1, PTR_, ARRAY, FN, CHAR_ };

struct Type {
	enum TypeCategory type_category;
	struct Type *derived_from;
	int array_length;
	struct ParamInfos param_infos;
};

struct ParamInfo {
	struct Type param_type;
	const char *ident_str;
};

int size_of(struct Type type);

extern struct Type INT_TYPE;

void expect_type(struct Type actual_type, struct Type expected_type,
                 const char *message);

struct Type deref_type(struct Type t);
int is_compatible(struct Type t1, struct Type t2);
int is_pointer(struct Type t);
struct Type ptr_of_type_to_ptr_to_type(struct Type *ptr_type);

void expect_and_consume(const struct Token **ptr_tokvec, enum TokenKind kind,
                        const char *str);
_Noreturn void error_unexpected_token(const struct Token *tokvec,
                                      const char *str);

struct Token *read_all_tokens(const char *str);

void debug_print_type(struct Type type);

struct FuncInfo {
	struct Type ret_type;
	struct ParamInfos param_infos;
};

int can_start_a_type(const struct Token *tokvec);

struct Type parse_declarator(const struct Token **ptr_tokvec,
                             const char **ptr_to_ident_str);

_Noreturn void unsupported(const char *str);

enum ExprCategory {
	SIMPLE_BINARY_EXPR = 1,
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
	SIMPLE_BIN_OP_PLUS = 1,
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
	UNARY_OP_NOT = 1,
	UNARY_OP_TILDA,
	UNARY_OP_PLUS,
	UNARY_OP_MINUS,

	UNARY_OP_PLUS_PLUS,
	UNARY_OP_MINUS_MINUS,

	UNARY_OP_AND,

	UNARY_OP_ASTERISK
};

struct Expression {
	struct {
		struct Type type;
		struct Type true_type;
	} details;
	enum ExprCategory category;
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
	int local_var_offset;
};

enum SimpleBinOp to_simplebinop(enum TokenKind t);
struct Expression simple_binary_op(struct Expression expr,
                                   struct Expression expr2, enum TokenKind kind,
                                   struct Type type);

enum UnaryOp to_unaryop(enum TokenKind t);

int is_array(struct Type t);
struct Type if_array_convert_to_ptr(struct Type t);
enum SimpleBinOp op_before_assign(enum TokenKind kind);

void read_all_tokens_debug(const char *str);

struct Type ptr_of_type_to_arr_of_type(struct Type *ptr_type, int length);

extern struct Type CHAR_TYPE;

struct Expression combine_by_add(struct Expression expr,
                                 struct Expression expr2);

enum StatementCategory {
	COMPOUND_STATEMENT = 1,
	IF_STATEMENT,
	IF_ELSE_STATEMENT,
	FOR_STATEMENT,
	WHILE_STATEMENT,
	DO_WHILE_STATEMENT,
	RETURN_STATEMENT,
	BREAK_STATEMENT,
	CONTINUE_STATEMENT,
	EXPRESSION_STATEMENT,
	DECLARATION_STATEMENT,
};

struct Statement {
	enum StatementCategory category;
	struct Vector statement_vector;
	struct Expression expr1;
	struct Expression expr2;
	struct Expression expr3;
	struct Statement *inner_statement;
	struct {
		struct Type type;
		const char *ident_str;
	} declaration;
};

enum ToplevelCategory {
	TOPLEVEL_VAR_DEFINITION,
	TOPLEVEL_FUNCTION_DEFINITION,
	TOPLEVEL_FUNCTION_DECLARATION
};

struct Toplevel {
	enum ToplevelCategory category;
	const char *declarator_name;
	struct Type declarator_type;
	struct {
		struct Statement sta;
		struct Vector offsets_and_types;
		struct Type ret_type;
		int capacity;
	} func;
};
