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
void gen_pop_to_reg_4byte(const char *str);
void gen_write_register_to_local_4byte(const char *str, int offset);
void gen_label(int label1);
void gen_discard(void);
void gen_push_address_of_local(int offset);
void gen_write_to_local_8byte(int offset);
void gen_jump(int label, const char *comment);
void gen_discard2nd_8byte(void);
void gen_pop_to_reg_8byte(const char *str);
void gen_write_register_to_local_8byte(const char *str, int offset);
void gen_cltq(void);
void gen_op_8byte(const char *str);
void gen_mul_by_const(int mul);
void gen_swap(void);
void gen_div_by_const(int num);
void gen_global_declaration(const char *ident, int size);
void gen_push_address_of_global(const char *ident);
void gen_write_register_to_local_1byte(const char *str, int offset);
void gen_string_literal(int strnum, const char *str);
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
void gen_call_and_assign_small_struct_to_local(const char *fname, int offset,
                                               int size);
void gen_logical_not_of_pointer(void);
void gen_call_reg_and_push_ret_of_nbyte(int n, const char *reg);
void return_garbage(void);

#ifdef OSX
#define PREFIX "_"
#endif
#ifndef OSX
#define PREFIX ""
#endif
