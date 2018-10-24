/* prologue and epilogue */
void gen_prologue(int alloc_size, const char *fname);
void gen_prologue_static(int alloc_size, const char *fname);
void gen_epilogue_nbyte(int n, int label_name);
void gen_epilogue_returning_small_struct(int size, int label);
void return_garbage(void);

/* push: decreases stack pointer by 8 */
void gen_push_int(int num);
void gen_push_address_of_local(int offset);
void gen_push_address_of_str(int strnum);
void gen_push_from_local_nbyte(int n, int offset);
void gen_push_address_of_global(const char *ident);
void gen_push_ret_of_nbyte(int n, const char *ident_str);
void gen_push_nullptr(void);
void gen_call_reg_and_push_ret_of_nbyte(int n, const char *reg);
void gen_peek_deref_push_nbyte(int n);

/* control: contains jump or is a label. does not tweak the stack pointer. */
void gen_jump(int label, const char *comment);
void gen_if_zero_jmp_nbyte(int n, int label1, int offset);
void gen_if_nonzero_jmp_nbyte(int n, int label1, int offset);
void gen_if_2nd_matches_int_jmp_4byte(int constant1, int label1);
void gen_label(int label1);

/* binary operation: pops twice, does an operation and pushes the result */
void gen_op_ints(const char *str);
void gen_compare_ints(const char *str);
void gen_mul_ints(void);
void gen_div_ints(void);
void gen_rem_ints(void);
void gen_shift_ints(const char *str);
void gen_discard(void);
void gen_discard2nd(void);
void gen_compare_ptrs(const char *str);
void gen_assign_nbyte(int n);
void gen_op_8byte(const char *str);

/* unary operation: pops once, does an operation and pushes the result */
void gen_cltq(void);
void gen_unary_not(void);
void gen_unary(const char *str);
void gen_extend_to_4byte(void);
void gen_mul_by_const(int mul);
void gen_div_by_const(int num);
void gen_logical_not_of_pointer(void);
void gen_peek_and_dereference_nbyte(int n);

/* declaration: declares something */
void gen_string_literal(int strnum, const char *str);
void gen_global_declaration(const char *ident, int size);

/* pop to reg: pops to a register. */
void gen_pop_to_reg_4byte(const char *str);
void gen_pop_to_reg_8byte(const char *str);

/* write to local: does not modify the stack */
void gen_write_register_to_local_1byte(const char *str, int offset);
void gen_write_register_to_local_4byte(const char *str, int offset);
void gen_write_register_to_local_8byte(const char *str, int offset);
void gen_call_and_assign_small_struct_to_local(const char *fname, int offset,
                                               int size);

/* memcpy: pops two pointers and does memcpy */
void gen_copy_2nd_struct_to_1st_and_discard(int size);
void gen_copy_1st_struct_to_2nd_and_discard(int size);

/* regname */
const char *get_reg_name_from_arg_pos_4byte(int counter);
const char *get_reg_name_from_arg_pos_8byte(int counter);

#ifdef OSX
#define PREFIX "_"
#endif
#ifndef OSX
#define PREFIX ""
#endif
