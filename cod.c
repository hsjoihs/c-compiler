struct Map2;

void insert(struct Map2 *map, const char *key, void *value);
void *lookup(const struct Map2 *map, const char *key);
struct Map2 *init_map(void);
int isElem(const struct Map2 *map, const char *key);
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

static void print_op_pointer_plusminus_int(int is_plus, int size)
{
 gen_cltq();
 gen_mul_by_const(size);
 gen_op_8byte(is_plus ? "addq" : "subq");
}


static void print_simple_binary_op(enum SimpleBinOp kind,
                                   const struct Type *ref_left_type, int size)
{
 const struct Type left_type = *ref_left_type;
 assert(left_type.type_category != STRUCT_);
 if (left_type.type_category == PTR_) {
  switch (kind) {
  case SIMPLE_BIN_OP_PLUS:
   print_op_pointer_plusminus_int(1, size);
   return;
  case SIMPLE_BIN_OP_MINUS:
   print_op_pointer_plusminus_int(0, size);
   return;

  case SIMPLE_BIN_OP_EQ_EQ:
   gen_compare_ptrs("sete");
   return;
  case SIMPLE_BIN_OP_NOT_EQ:
   gen_compare_ptrs("setne");
   return;
  case SIMPLE_BIN_OP_LT_EQ:
   gen_compare_ptrs("setbe");
   return;
  case SIMPLE_BIN_OP_GT_EQ:
   gen_compare_ptrs("setnb");
   return;
  case SIMPLE_BIN_OP_LT:
   gen_compare_ptrs("setb");
   return;
  case SIMPLE_BIN_OP_GT:
   gen_compare_ptrs("seta");
   return;
  default:
   simple_error("unexpected pointer in binary operation\n");
  }
 }
 switch (kind) {
 case SIMPLE_BIN_OP_PLUS:
  gen_op_ints("addl");
  return;
 case SIMPLE_BIN_OP_MINUS:
  gen_op_ints("subl");
  return;
 case SIMPLE_BIN_OP_ASTERISK:
  gen_mul_ints();
  return;
 case SIMPLE_BIN_OP_SLASH:
  gen_div_ints();
  return;
 case SIMPLE_BIN_OP_PERCENT:
  gen_rem_ints();
  return;
 case SIMPLE_BIN_OP_LT:
  gen_compare_ints("setl");
  return;
 case SIMPLE_BIN_OP_LT_EQ:
  gen_compare_ints("setle");
  return;
 case SIMPLE_BIN_OP_LSHIFT:
  gen_shift_ints("sall");
  return;
 case SIMPLE_BIN_OP_GT:
  gen_compare_ints("setg");
  return;
 case SIMPLE_BIN_OP_GT_EQ:
  gen_compare_ints("setge");
  return;
 case SIMPLE_BIN_OP_RSHIFT:
  gen_shift_ints("sarl");
  return;
 case SIMPLE_BIN_OP_AND:
  gen_op_ints("andl");
  return;
 case SIMPLE_BIN_OP_OR:
  gen_op_ints("orl");
  return;
 case SIMPLE_BIN_OP_EQ_EQ:
  gen_compare_ints("sete");
  return;
 case SIMPLE_BIN_OP_NOT_EQ:
  gen_compare_ints("setne");
  return;
 case SIMPLE_BIN_OP_HAT:
  gen_op_ints("xorl");
  return;
 }
}

void print_address_of_lvalue_or_struct(struct PrinterState *ptr_prs,
                                       const struct Expr *ref_expr,
                                       const char *msg)
{
 const struct Expr expr = *ref_expr;
 switch (expr.category) {
 case CONDITIONAL_EXPR: {
  int label1 = get_new_label_name(ptr_prs);
  int label2 = get_new_label_name(ptr_prs);

  print_expression(ptr_prs, expr.ptr1);
  gen_if_zero_jmp_nbyte(
      size_of_basic(&expr.ptr1->details.type,
                    "condition of conditional expression"),
      label1, 0);

  if (expr.ptr2->details.type.type_category != STRUCT_) {
   simple_error("the conditional operator is used as lvalue");
  }

  print_address_of_lvalue_or_struct(ptr_prs, expr.ptr2,
                                    "true branch of ternary operator");
  gen_jump(label2, "ternary operator");
  gen_label(label1);
  print_address_of_lvalue_or_struct(ptr_prs, expr.ptr3,
                                    "false branch of ternary operator");
  gen_label(label2);
  gen_discard2nd_8byte();
  return;
 }
 case STRUCT_ASSIGNMENT_EXPR: {
  print_address_of_lvalue_or_struct(ptr_prs, expr.ptr1,
                                    "left hand of struct assignment");
  print_address_of_lvalue_or_struct(ptr_prs, expr.ptr2,
                                    "right hand of struct assignment");
  int size = expr.size_info_for_struct_assign;
  gen_copy_struct_and_discard(size);
  print_address_of_lvalue_or_struct(ptr_prs, expr.ptr1,
                                    "left hand of struct assignment");
  return;
 }
 case FUNCCALL_EXPR_RETURNING_INTEGER_CLASS: {

  pass_args(ptr_prs, &expr.args);

  int size = expr.size_info_for_struct_assign;
  gen_call_and_assign_small_struct_to_local(expr.global_var_name,
                                            expr.local_var_offset, size);
  gen_push_address_of_local(expr.local_var_offset);
  return;
 }
 case FUNCCALL_EXPR_RETURNING_MEMORY_CLASS: {

  pass_args(ptr_prs, &expr.args);


  gen_push_ret_of_nbyte(4, expr.global_var_name);
  gen_discard();

  gen_push_address_of_local(expr.local_var_offset);
  return;
 }
 case STRUCT_AND_OFFSET: {
  print_address_of_lvalue_or_struct(
      ptr_prs, expr.ptr1, "struct whose member is to be accessed");
  int offset = expr.struct_offset;
  gen_push_int(offset);
  gen_cltq();
  gen_op_8byte("addq");
  return;
 }
 case LOCAL_VAR_: {
  gen_push_address_of_local(expr.local_var_offset);

  return;
 }
 case GLOBAL_VAR_: {
  gen_push_address_of_global(expr.global_var_name);

  return;
 }
 case UNARY_OP_EXPR:
  switch (expr.unary_operator) {
  case UNARY_OP_ASTERISK: {
   print_expression(ptr_prs, expr.ptr1);
   return;
  }
  default:
   simple_error("the only unary operator that can create "
                "lvalue is `*`\n");
  }
 case VOID_EXPR:
  fprintf(stderr, "context: %s\n", msg);
  simple_error("doesn't seem like an lvalue; it is a void expr.\n");
 case COMMA_EXPR: {
  print_expression_or_addr_of_struct(
      ptr_prs, expr.ptr1, "struct as the first operand of comma");

  if (expr.ptr2->details.type.type_category != STRUCT_) {
   simple_error(
       "either the result of comma expression is used as an "
       "lvalue, or the comma expression is used as if it were a "
       "struct though the second operand of comma isn't.\n");
  }
  print_address_of_lvalue_or_struct(
      ptr_prs, expr.ptr2, "struct as the second operand of comma");

  gen_discard2nd_8byte();
  return;
 }
 default:
  fprintf(stderr, "context: %s\n", msg);
  simple_error("doesn't seem like an lvalue or a struct\n");
 }
}

void print_expression(struct PrinterState *ptr_prs,
                      const struct Expr *ref_expr);
static void print_expression_as_lvalue(struct PrinterState *ptr_prs,
                                       const struct Expr *ref_expr);

static void print_expression_as_lvalue(struct PrinterState *ptr_prs,
                                       const struct Expr *ref_expr)
{
 const struct Expr expr = *ref_expr;
 print_address_of_lvalue_or_struct(ptr_prs, &expr, "as lvalue");
 switch (expr.category) {
 case FUNCCALL_EXPR_RETURNING_INTEGER_CLASS: {
  unsupported("FUNCCALL_EXPR_RETURNING_STRUCT");
 }
 case FUNCCALL_EXPR_RETURNING_MEMORY_CLASS: {
  unsupported("FUNCCALL_EXPR_RETURNING_STRUCT");
 }
 case STRUCT_AND_OFFSET: {
  gen_peek_deref_push_nbyte(
      size_of_basic(&expr.details.type, "foo.bar as lvalue"));
  return;
 }
 case LOCAL_VAR_: {
  gen_push_from_local_nbyte(
      size_of_basic(&expr.details.type, "local var as lvalue"),
      expr.local_var_offset);

  return;
 }
 case GLOBAL_VAR_: {
  printf("//load from global `%s`\n", expr.global_var_name);
  gen_peek_deref_push_nbyte(
      size_of_basic(&expr.details.type, "global var as lvalue"));

  return;
 }
 case UNARY_OP_EXPR:
  switch (expr.unary_operator) {
  case UNARY_OP_ASTERISK: {
   gen_peek_deref_push_nbyte(
       size_of_basic(&expr.details.type, "*expr as lvalue"));
   return;
  }
  default:
   simple_error("the only unary operator that can create "
                "lvalue is `*`\n");
  }
 default:
  simple_error("doesn't seem like an lvalue\n");
 }
}

void print_expression(struct PrinterState *ptr_prs, const struct Expr *ref_expr)
{
 const struct Expr expr = *ref_expr;
 switch (expr.category) {
 case FUNCCALL_EXPR_RETURNING_INTEGER_CLASS: {
  unsupported("FUNCCALL_EXPR_RETURNING_STRUCT");
 }
 case FUNCCALL_EXPR_RETURNING_MEMORY_CLASS: {
  unsupported("FUNCCALL_EXPR_RETURNING_STRUCT");
 }
 case STRUCT_ASSIGNMENT_EXPR: {
  print_address_of_lvalue_or_struct(ptr_prs, expr.ptr1,
                                    "left hand of struct assignment");
  print_address_of_lvalue_or_struct(ptr_prs, expr.ptr2,
                                    "right hand of struct assignment");
  int size = expr.size_info_for_struct_assign;
  gen_copy_struct_and_discard(size);
  gen_push_int(143253);
  return;
 }
 case VOID_EXPR: {
  gen_push_int(123);
  return;
 }
 case NULLPTR: {
  gen_push_nullptr();
  return;
 }
 case STRUCT_AND_OFFSET: {
  print_expression_as_lvalue(ptr_prs, &expr);
  gen_discard2nd_8byte();
  return;
 }
 case POINTER_MINUS_POINTER: {
  print_expression(ptr_prs, expr.ptr1);
  print_expression(ptr_prs, expr.ptr2);
  int size = expr.size_info_for_pointer_arith;
  gen_op_8byte("subq");
  gen_div_by_const(size);
  return;
 }
 case POINTER_PLUS_INT:
 case POINTER_MINUS_INT: {
  int size = expr.size_info_for_pointer_arith;
  print_expression(ptr_prs, expr.ptr1);
  print_expression(ptr_prs, expr.ptr2);
  print_op_pointer_plusminus_int(expr.category == POINTER_PLUS_INT, size);
  return;
 }
 case POSTFIX_INCREMENT:
 case POSTFIX_DECREMENT: {
  enum SimpleBinOp opkind2 = expr.category == POSTFIX_INCREMENT
                                 ? SIMPLE_BIN_OP_PLUS
                                 : SIMPLE_BIN_OP_MINUS;

  print_expression_as_lvalue(ptr_prs, expr.ptr1);
  gen_push_int(1);

  print_simple_binary_op(opkind2, &expr.ptr1->details.type,
                         expr.size_info_for_pointer_arith);

  gen_assign_nbyte(
      size_of_basic(&expr.ptr1->details.type, "postfix inc/dec"));

  gen_push_int(-1);

  print_simple_binary_op(opkind2, &expr.ptr1->details.type,
                         expr.size_info_for_pointer_arith);

  return;
 }

 case LOCAL_VAR_: {
  if (expr.details.true_type.type_category == ARRAY) {
   gen_push_address_of_local(expr.local_var_offset);
   return;
  }
  gen_push_from_local_nbyte(
      size_of_basic(&expr.details.type, "local var as rvalue"),
      expr.local_var_offset);

  return;
 }

 case GLOBAL_VAR_: {
  printf("//global `%s` as rvalue\n", expr.global_var_name);

  if (expr.details.true_type.type_category == ARRAY) {
   gen_push_address_of_global(expr.global_var_name);
   return;
  }

  gen_push_address_of_global(expr.global_var_name);
  gen_peek_and_dereference_nbyte(
      size_of_basic(&expr.details.type, "global var as rvalue"));

  return;
 }

 case SIMPLE_BINARY_EXPR: {
  print_expression(ptr_prs, expr.ptr1);
  print_expression(ptr_prs, expr.ptr2);
  print_simple_binary_op(expr.simple_binary_operator,
                         &expr.ptr1->details.type,
                         expr.size_info_for_pointer_arith);
  return;
 }

 case COMMA_EXPR: {
  print_expression_or_addr_of_struct(
      ptr_prs, expr.ptr1, "struct as the first operand of comma");

  if (expr.ptr2->details.type.type_category == STRUCT_) {
   simple_error("struct is used as the right operand of comma "
                "operator, but the result of comma expression is used "
                "in a non-struct manner.\n");

  } else {
   print_expression(ptr_prs, expr.ptr2);
  }

  gen_discard2nd_8byte();
  return;
 }

 case LOGICAL_OR_EXPR: {

  int label1 = get_new_label_name(ptr_prs);
  int label2 = get_new_label_name(ptr_prs);
  print_expression(ptr_prs, expr.ptr1);

  gen_if_nonzero_jmp_nbyte(
      size_of_basic(&expr.ptr1->details.type, "operand of logical or"),
      label1, 0);
  print_expression(ptr_prs, expr.ptr2);
  gen_discard();
  gen_if_nonzero_jmp_nbyte(
      size_of_basic(&expr.ptr2->details.type, "operand of logical or"),
      label1, -8);
  gen_logical_OR_part2(label1, label2);
  return;
 }
 case LOGICAL_AND_EXPR: {

  int label1 = get_new_label_name(ptr_prs);
  int label2 = get_new_label_name(ptr_prs);
  print_expression(ptr_prs, expr.ptr1);

  gen_if_zero_jmp_nbyte(
      size_of_basic(&expr.ptr1->details.type, "operand of logical and"),
      label1, 0);
  print_expression(ptr_prs, expr.ptr2);
  gen_discard();
  gen_if_zero_jmp_nbyte(
      size_of_basic(&expr.ptr2->details.type, "operand of logical and"),
      label1, -8);
  gen_logical_AND_part2(label1, label2);
  return;
 }

 case ASSIGNMENT_EXPR: {

  print_expression_as_lvalue(ptr_prs, expr.ptr1);
  print_expression(ptr_prs, expr.ptr2);

  gen_discard2nd_8byte();

  struct Type type = expr.ptr1->details.type;

  gen_assign_nbyte(size_of_basic(&type, "operand of assignment"));

  return;
 }
 case COMPOUND_ASSIGNMENT_EXPR: {

  print_expression_as_lvalue(ptr_prs, expr.ptr1);
  print_expression(ptr_prs, expr.ptr2);

  print_simple_binary_op(expr.simple_binary_operator,
                         &expr.ptr1->details.type,
                         expr.size_info_for_pointer_arith);

  struct Type type = expr.ptr1->details.type;

  gen_assign_nbyte(size_of_basic(&type, "operand of assignment"));

  return;
 }
 case INT_VALUE:
  gen_push_int(expr.int_value);
  return;
 case UNARY_OP_EXPR:
  switch (expr.unary_operator) {
  case UNARY_OP_NOT: {
   print_expression(ptr_prs, expr.ptr1);
   if (size_of_basic(&expr.ptr1->details.type,
                     "operand of logical not") == 8) {
    gen_logical_not_of_pointer();
   } else {
    gen_unary_not();
   }
   return;
  }
  case UNARY_OP_TILDA:
   print_expression(ptr_prs, expr.ptr1);
   gen_unary("notl");
   return;
  case UNARY_OP_PLUS:
   print_expression(ptr_prs, expr.ptr1);

   return;
  case UNARY_OP_MINUS:
   print_expression(ptr_prs, expr.ptr1);
   gen_unary("negl");
   return;

  case UNARY_OP_PLUS_PLUS:
  case UNARY_OP_MINUS_MINUS: {
   print_expression_as_lvalue(ptr_prs, expr.ptr1);

   gen_push_int(1);
   print_simple_binary_op(
       expr.unary_operator == UNARY_OP_PLUS_PLUS ? SIMPLE_BIN_OP_PLUS
                                                 : SIMPLE_BIN_OP_MINUS,
       &expr.ptr1->details.type, expr.size_info_for_pointer_arith);

   struct Type type = expr.ptr1->details.type;

   gen_assign_nbyte(size_of_basic(&type, "prefix inc/dec"));
   return;
  }

  case UNARY_OP_AND: {
   struct Type type = expr.ptr1->details.type;
   struct Type true_type = expr.ptr1->details.true_type;
   if (type.type_category == PTR_ &&
       true_type.type_category == ARRAY) {
    print_expression(ptr_prs, expr.ptr1);
   } else {
    print_address_of_lvalue_or_struct(
        ptr_prs, expr.ptr1, "address requested by & operator");
   }
   return;
  }

  case UNARY_OP_ASTERISK: {
   print_expression(ptr_prs, expr.ptr1);
   struct Type type = expr.details.type;
   struct Type true_type = expr.details.true_type;

   if (type.type_category == PTR_ &&
       true_type.type_category == ARRAY) {

    return;
   }
   gen_peek_and_dereference_nbyte(
       size_of_basic(&type, "*expr as rvalue"));

   return;
  }
  }
  return;
 case CONDITIONAL_EXPR: {
  int label1 = get_new_label_name(ptr_prs);
  int label2 = get_new_label_name(ptr_prs);

  print_expression(ptr_prs, expr.ptr1);
  gen_if_zero_jmp_nbyte(
      size_of_basic(&expr.ptr1->details.type,
                    "condition of conditional expression"),
      label1, 0);
  print_expression(ptr_prs, expr.ptr2);
  gen_jump(label2, "ternary operator");
  gen_label(label1);
  print_expression(ptr_prs, expr.ptr3);
  gen_label(label2);
  gen_discard2nd_8byte();
  return;
 }
 case FUNCCALL_EXPR: {
  const char *ident_str = expr.global_var_name;
  struct Type ret_type = expr.details.type;

  pass_args(ptr_prs, &expr.args);

  int size;
  if (ret_type.type_category != VOID_) {
   size = size_of_basic(&ret_type, "return value");
  } else {
   size = 4;
  }
  gen_push_ret_of_nbyte(size, ident_str);

  return;
 }

 case STRING_LITERAL: {
  const char *str = expr.literal_string;
  push_vector(&ptr_prs->string_constant_pool, str);
  gen_push_address_of_str(ptr_prs->string_constant_pool.length - 1);
 }
 }
}

static void pass_args(struct PrinterState *ptr_prs,
                      const struct Vector *ref_args)
{
 for (int counter = ref_args->length - 1; counter >= 0; counter--) {
  const struct Expr *ptr_expr_ = ref_args->vector[counter];

  print_expression(ptr_prs, ptr_expr_);
 }

 for (int counter = 0; counter < ref_args->length; counter++) {
  const struct Expr *ptr_expr_ = ref_args->vector[counter];

  switch (size_of_basic(&ptr_expr_->details.type, "argument")) {
  case 1:
  case 4:
   gen_pop_to_reg_4byte(get_reg_name_from_arg_pos_4byte(counter));
   break;
  case 8:
   gen_pop_to_reg_8byte(get_reg_name_from_arg_pos_8byte(counter));
   break;
  default:
   unsupported("Unsupported width in function argument");
  }
 }
}

void print_expression_or_addr_of_struct(struct PrinterState *ptr_prs,
                                        const struct Expr *ref_expr,
                                        const char *msg)
{
 if (ref_expr->details.type.type_category == STRUCT_) {
  print_address_of_lvalue_or_struct(ptr_prs, ref_expr, msg);
 } else {
  print_expression(ptr_prs, ref_expr);
 }
}
