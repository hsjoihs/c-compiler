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
	LIT_STRING,
	RES_STRUCT,
	RES_SIZEOF,
	DOT,
	ARROW,
	RES_VOID,
	RES_SWITCH,
	RES_CASE,
	RES_DEFAULT
};

struct Token {
	enum TokenKind kind;
	int int_value;
	const char *ident_str;
	const char *literal_str;
	const char *token_begins_here;
};

void print_token(struct Token tok, const char *next_token_begins);
struct Token get_token(const char **ptr_to_str);

struct TypeAndIdent;

enum TypeCategory { INT_ = 1, PTR_, ARRAY, FN, CHAR_, STRUCT_, VOID_ };

struct StructInternalInfo {
	struct Vector /* <TypeAndIdent> */ types_and_idents;
};

struct SizeAndAlignment {
	int size;
	int alignment;
};

struct StructInternalCompleteInfo {
	struct StructInternalInfo info;
	int *offset_vec;
	struct SizeAndAlignment s_and_a;
};

struct Type {
	enum TypeCategory type_category;
	struct Type *derived_from;
	int array_length;
	struct Vector param_infos;
	/*
	 .vector points to the array of (TypeAndIdent*).
	 if .vector itself is NULL, that means there is no info.
	 */
	struct {
		const char *struct_tag;
		struct StructInternalInfo struct_info;
	};
};

struct TypeAndIdent {
	struct Type type;
	const char *ident_str;
};

int size_of_basic(struct Type type);

struct Type INT_TYPE(void);

struct Type deref_type(struct Type t);
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
	struct Vector param_infos;
	/*
	 .vector points to the array of (TypeAndIdent*).
	 if .vector itself is NULL, that means there is no info.
	 */
};

int can_start_a_type(const struct Token *tokvec);

struct Type parse_declarator(const struct Token **ptr_tokvec,
                             const char **ptr_to_ident_str);
struct Type parse_type_name(const struct Token **ptr_tokvec);

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
	STRING_LITERAL,
	STRUCT_AND_OFFSET,
	NULLPTR,
	VOID_EXPR
};

enum UntypedExprCategory {
	BINARY_EXPR = 1,
	UNARY_EXPR,
	CONDITIONAL,
	VAR,
	INT_LITERAL_,
	POSTFIX_EXPR,
	FUNCCALL,
	STRING_LITERAL_,
	SIZEOF_TYPE,
	DOT_EXPR
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

struct UntypedExpression {
	enum TokenKind operator;
	enum UntypedExprCategory category;
	struct UntypedExpression *ptr1;
	struct UntypedExpression *ptr2;
	struct UntypedExpression *ptr3;
	int int_value;
	const char *var_name;
	const char *literal_string;
	struct Vector /* <UntypedExpression> */ arg_exprs_vec;
	struct Type operand_of_sizeof;
	const char *ident_after_dot;
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

	/* used in POINTER_PLUS_INT, POINTER_MINUS_INT and POINTER_MINUS_POINTER */
	int size_info_for_pointer_arith;

	int struct_offset;
};

int is_array(struct Type t);
struct Type if_array_convert_to_ptr(struct Type t);

void read_all_tokens_debug(const char *str);

struct Type ptr_of_type_to_arr_of_type(struct Type *ptr_type, int length);

struct Type CHAR_TYPE(void);

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
	SWITCH_STATEMENT
};

enum SourceLabelCategory { DEFAULT_LABEL, CASE_LABEL, IDENT_LABEL };

struct SourceLabel {
	enum SourceLabelCategory category;
	int case_int;
	const char *ident_str;
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
	struct Vector /*<SourceLabel>*/ labels;
};

enum ToplevelCategory {
	TOPLEVEL_VAR_DEFINITION = 1,
	TOPLEVEL_FUNCTION_DEFINITION,
	TOPLEVEL_FUNCTION_DECLARATION,
};

struct Toplevel {
	enum ToplevelCategory category;
	const char *declarator_name;
	/* NULL when TOPLEVEL_VAR_DEFINITION does not have an identifier */

	struct Type declarator_type;
	/* used when it is TOPLEVEL_VAR_DEFINITION or TOPLEVEL_TYPE_DECLARATION */

	struct {
		struct Statement sta;
		struct Vector offsets_and_types;
		struct Type ret_type;
		int capacity;
	} func;

	int size_of_declarator_type;
};

struct UntypedExpression parse_expression(const struct Token **ptr_tokvec);
int isAssign(enum TokenKind opkind);
struct Type *parse_type_specifier(const struct Token **ptr_tokvec);

struct SizeAndAlignment
get_size_alignment_offsets(const struct SizeAndAlignment *inner_type_vec,
                           int **ptr_offset_vec, int length);

struct Type *
try_parse_type_specifier_and_semicolon(const struct Token **ptr_tokvec);

struct UntypedExpression
parse_constant_expression(const struct Token **ptr_tokvec);
