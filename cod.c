struct Vector {
 int length;
 int _allocated_length;
 const void **vector;
};
struct Vector *init_vector_(void);
void extend_vector(struct Vector *ptr);
void push_vector(struct Vector *ptr, const void *tok);
const void *pop_vector(struct Vector *ptr);
struct Vector init_vector(void);
void concat_vector(struct Vector *ptr_ans, const struct Vector *vec);

enum TokenKind {
 OP_PLUS ,
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
 RES_DEFAULT,
 RES_ALIGNOF,
 RES_ENUM,
 RES_CONST,
 RES_NORETURN,
 RES_EXTERN,
 RES_STATIC,


 HASH,
 NEWLINE,
 SPACE
};

struct Token {
 enum TokenKind kind;
 int int_value;
 const char *ident_str;
 const char *literal_str;
 const char *token_begins_here;
};

enum TypeCategory {
 INT_ ,
 PTR_,
 ARRAY,
 FN,
 CHAR_,
 STRUCT_,
 VOID_,
 ENUM_
};

struct TypeAndIdent;

struct StructInternalInfo {
 struct Vector *ptr_types_and_idents;
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

struct Enumerators {
 struct Vector *ptr_enumerators;
};

struct StructTagAndInfo {
 const char *struct_tag;
 struct StructInternalInfo struct_info;
};

struct EnumTagAndInfo {
 const char *enum_tag;
 struct Enumerators enum_info;
};

struct Type {
 enum TypeCategory type_category;
 struct Type *derived_from;
 int array_length;
 struct Vector param_infos;
 int is_param_infos_valid;

 struct StructTagAndInfo s;
 struct EnumTagAndInfo e;
};

struct TypeAndIdent {
 struct Type type;
 const char *ident_str;
};

int size_of_basic(const struct Type *ref_type, const char *msg);

struct Type INT_TYPE(void);

struct Type deref_type(const struct Type *ref_t);

void expect_and_consume(const struct Token **ptr_tokvec, enum TokenKind kind,
                        const char *str);
_Noreturn void error_unexpected_token(const struct Token *tokvec,
                                      const char *str);

struct Token *read_all_tokens(const char *str);

void debug_print_type(const struct Type *ref_type);

int can_start_a_type(const struct Token *tokvec);

struct Type parse_type_name(const struct Token **ptr_tokvec);

_Noreturn void unsupported(const char *str);

enum ExprCategory {
 SIMPLE_BINARY_EXPR ,
 COMMA_EXPR,
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
 COMPOUND_ASSIGNMENT_EXPR,
 ASSIGNMENT_EXPR,
 LOGICAL_OR_EXPR,
 LOGICAL_AND_EXPR,
 STRING_LITERAL,
 STRUCT_AND_OFFSET,
 NULLPTR,
 VOID_EXPR,
 STRUCT_ASSIGNMENT_EXPR,
 FUNCCALL_EXPR_RETURNING_INTEGER_CLASS,
 FUNCCALL_EXPR_RETURNING_MEMORY_CLASS,
};

enum UntypedExprCategory {
 BINARY_EXPR ,
 UNARY_EXPR,
 CONDITIONAL,
 VAR,
 INT_LITERAL_,
 POSTFIX_EXPR,
 FUNCCALL,
 STRING_LITERAL_,
 SIZEOF_TYPE,
 ALIGNOF_TYPE,
 DOT_EXPR
};

enum SimpleBinOp {
 SIMPLE_BIN_OP_PLUS ,
 SIMPLE_BIN_OP_MINUS,
 SIMPLE_BIN_OP_ASTERISK,
 SIMPLE_BIN_OP_SLASH,
 SIMPLE_BIN_OP_PERCENT,
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
 UNARY_OP_NOT ,
 UNARY_OP_TILDA,
 UNARY_OP_PLUS,
 UNARY_OP_MINUS,

 UNARY_OP_PLUS_PLUS,
 UNARY_OP_MINUS_MINUS,

 UNARY_OP_AND,

 UNARY_OP_ASTERISK
};

struct UntypedExpr {
 enum TokenKind operator_;
 enum UntypedExprCategory category;
 struct UntypedExpr *ptr1;
 struct UntypedExpr *ptr2;
 struct UntypedExpr *ptr3;
 int int_value;
 const char *var_name;
 const char *literal_string;
 struct Vector arg_exprs_vec;
 struct Type operand_of_sizeof_or_alignof;
 const char *ident_after_dot;
};

struct Type parse_declaration(const struct Token **ptr_tokvec,
                              const char **ptr_to_ident_str,
                              struct UntypedExpr **ptr_ptr_uexpr);

struct TypePair {
 struct Type type;
 struct Type true_type;
};

struct Expr {
 struct TypePair details;
 enum ExprCategory category;
 enum SimpleBinOp simple_binary_operator;
 enum UnaryOp unary_operator;
 struct Expr *ptr1;
 struct Expr *ptr2;
 struct Expr *ptr3;
 int int_value;
 const char *global_var_name;
 const char *literal_string;
 struct Vector args;
 int local_var_offset;


 int size_info_for_pointer_arith;


 int size_info_for_struct_assign;

 int struct_offset;
};

struct Type ptr_of_type_to_arr_of_type(struct Type *ptr_type, int length);

struct Type CHAR_TYPE(void);

enum StatementCategory {
 COMPOUND_STATEMENT ,
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
 struct Expr expr1;
 struct Expr expr2;
 struct Expr expr3;
 struct Statement *inner_statement;
 struct TypeAndIdent declaration;
 struct Vector labels;
};

struct UntypedExpr parse_expression(const struct Token **ptr_tokvec);
int isAssign(enum TokenKind opkind);
struct Type *parse_type_specifier(const struct Token **ptr_tokvec);

struct SizeAndAlignment
get_size_alignment_offsets(const struct SizeAndAlignment *inner_type_vec,
                           int **ptr_offset_vec, int length);

struct Type *
try_parse_type_specifier_and_semicolon(const struct Token **ptr_tokvec);

struct UntypedExpr parse_constant_expression(const struct Token **ptr_tokvec);

struct UntypedExpr parse_assignment_expression(const struct Token **ptr_tokvec);

struct Type parse_struct_declaration(const struct Token **ptr_tokvec,
                                     const char **ptr_to_ident_str);

struct UntypedExpr binary_op_untyped(const struct UntypedExpr *ref_expr,
                                     const struct UntypedExpr *ref_expr2,
                                     enum TokenKind kind);

char *unescape(const char *str);
char *escape(const char *str);

struct Type parse_type_specifier_and_declarator(const struct Token **ptr_tokvec,
                                                const char **ptr_to_ident_str);

_Noreturn void simple_error(const char *str);

void if_array_convert_to_ptr_(struct Type *ptr_t);

void print_token_at(const struct Token *tokvec);

struct Type ptr_to_type(const struct Type *ref_type);

struct Type arr_of_type(const struct Type *ref_type, int length);

struct Token *concat_str_literals(struct Token *tokvec);

void print_token(const struct Token *ptr_tok, const char *next_token_begins);

struct Token *remove_spaces_and_newlines(struct Token *tokvec);
void gen_prologue(int alloc_size, const char *fname);
void gen_push_int(int num);
void gen_op_ints(const char *str);
void gen_compare_ints(const char *str);
void gen_mul_ints(void);
void gen_div_ints(void);
void gen_rem_ints(void);
void gen_shift_ints(const char *str);
void gen_unary_not(void);
void gen_unary(const char *str);
void gen_push_from_local_4byte(int offset);
void gen_write_to_local(int offset);
void gen_epilogue(int label);
void gen_push_ret_of_4byte(const char *fname);
void gen_pop_to_reg_4byte(const char *str);
void gen_write_register_to_local_4byte(const char *str, int offset);
void gen_label(int label1);
void gen_while_part2(int label1, int label2);
void gen_discard(void);
void gen_push_address_of_local(int offset);
void gen_write_to_local_8byte(int offset);
void gen_push_from_local_8byte(int offset);
void gen_peek_and_dereference_4byte(void);
void gen_peek_and_dereference_8byte(void);
void gen_jump(int label, const char *comment);
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
void gen_global_declaration(const char *ident, int size);
void gen_push_address_of_global(const char *ident);
void gen_peek_deref_push_4byte(void);
void gen_peek_deref_push_8byte(void);
void gen_assign_8byte(void);
void gen_assign_4byte(void);
void gen_discard3rd_8byte(void);
void gen_push_from_local_1byte(int offset);
void gen_write_to_local_1byte(int offset);
void gen_push_ret_of_1byte(const char *fname);
void gen_peek_and_dereference_1byte(void);
void gen_assign_1byte(void);
void gen_peek_deref_push_1byte(void);
void gen_write_register_to_local_1byte(const char *str, int offset);
void gen_str(int strnum, const char *str);
void gen_push_address_of_str(int strnum);
void gen_logical_OR_part2(int label1, int label2);
void gen_logical_AND_part2(int label1, int label2);
void gen_push_from_local_nbyte(int n, int offset);
void gen_peek_deref_push_nbyte(int n);
void gen_assign_nbyte(int n);
void gen_peek_and_dereference_nbyte(int n);
void gen_push_ret_of_nbyte(int n, const char *ident_str);
void gen_epilogue_nbyte(int n, int label_name);
const char *get_reg_name_from_arg_pos_4byte(int counter);
const char *get_reg_name_from_arg_pos_8byte(int counter);
void gen_push_nullptr(void);
void gen_if_zero_jmp_nbyte(int n, int label1, int offset);
void gen_if_nonzero_jmp_nbyte(int n, int label1, int offset);
void gen_if_neg8_matches_jmp_4byte(int constant1, int label1);
void gen_extend_to_4byte(void);
void gen_compare_ptrs(const char *str);
void gen_copy_struct_and_discard(int size);
void gen_prologue_static(int alloc_size, const char *fname);
void gen_epilogue_returning_small_struct(int size, int label);
void gen_call(const char *fname);
void gen_call_and_assign_small_struct_to_local(const char *fname, int offset,
                                               int size);
void gen_logical_not_of_pointer(void);
struct PrinterState {
 int final_label_name;
 int return_label_name;
 int break_label_name;
 int continue_label_name;

 struct Vector string_constant_pool;
 int pool_largest_id;
 int is_inside_switch;
 struct Vector case_default_vec;
};

int get_new_label_name(struct PrinterState *ptr_prs);
void print_expression(struct PrinterState *ptr_prs,
                      const struct Expr *ref_expr);

void print_address_of_lvalue_or_struct(struct PrinterState *ptr_prs,
                                       const struct Expr *ref_expr,
                                       const char *msg);

void print_expression_or_addr_of_struct(struct PrinterState *ptr_prs,
                                        const struct Expr *ref_expr,
                                        const char *msg);
void assert();
_Noreturn void assert0(int i);
void *calloc(int nmemb, int size);
void *realloc(void *ptr, int size);
_Noreturn void exit(int status);

int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *s1, const char *s2);
char *strcat(char *s1, const char *s2);
char *strchr(const char *s, int c);
extern struct __FILE *stderr;
extern struct __FILE *stdin;
int fprintf();
int printf();
int sprintf();


char *fgets(char *buf, int a, struct __FILE *fp);
int puts(const char *str);
struct __FILE *fopen(const char *filename, const char *mode);

static void pass_args(struct PrinterState *ptr_prs,
                      const struct Vector *ref_args);

static void print_op_pointer_plusminus_int(int is_plus, int size);

void print_expression(struct PrinterState *ptr_prs,
                      const struct Expr *ref_expr);

