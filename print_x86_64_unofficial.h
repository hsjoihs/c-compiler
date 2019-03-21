/* auxiliary functions to implement the API */

void gen_raw_call_partB(const char *s1, const char *s2);
void gen_raw_call_partC();

/* unofficial API */
void gen_epilogue(int label);
void gen_epilogue_8byte(int label);
void gen_push_from_local_1byte(int offset);
void gen_push_from_local_4byte(int offset);
void gen_push_from_local_8byte(int offset);
void gen_push_ret_of_1byte(const char *fname);
void gen_push_ret_of_4byte(const char *fname);
void gen_push_ret_of_8byte(const char *fname);
void gen_assign_1byte(void);
void gen_assign_4byte(void);
void gen_assign_8byte(void);
void gen_peek_and_dereference_1byte(void);
void gen_peek_and_dereference_4byte(void);
void gen_peek_and_dereference_8byte(void);
void gen_call_reg_and_push_ret_of_1byte(const char *reg);
void gen_call_reg_and_push_ret_of_4byte(const char *reg);
void gen_call_reg_and_push_ret_of_8byte(const char *reg);
void gen_peek_deref_push_1byte(void);
void gen_peek_deref_push_4byte(void);
void gen_peek_deref_push_8byte(void);
void gen_swap(void);
void gen_write_to_local(int offset);
void gen_write_to_local_8byte(int offset);
void gen_write_to_local_1byte(int offset);
