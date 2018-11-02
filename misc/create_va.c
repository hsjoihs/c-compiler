#include "../print_x86_64.h"
#include "../print_x86_64_unofficial.h"
#include <stdio.h>

void gen_store_regs_to_local(int offset, int start_from, const char *label_name)
{
	puts("  testb %al, %al");
	for (int i = start_from; i <= 5; i++) {
		printf("  movq %%%s, %d(%%rbp)\n", get_reg_name_from_arg_pos_8byte(i),
		       offset + i * 8);
	}
	printf("  je %s\n", label_name);
	for (int i = 0; i < 8; i++) {
		printf("  movaps %%xmm%d, %d(%%rbp)\n", i, 8 + offset + 40 + 16 * i);
	}
	printf("%s:\n", label_name);
}

void gen_push_8byte(const char *num_as_str)
{
	printf("  subq $8, %%rsp\n"
	       "  movq $%s, %%rdx\n"
	       "  movq %%rdx, (%%rsp)\n",
	       num_as_str);
}

void gen_write_local_to_register_8byte(int offset, const char *str)
{
	printf("  movq %d(%%rbp), %%%s\n", offset, str);
}

void gen_write_to_reg_8byte(const char *str)
{
	printf("  movq (%%rsp), %%%s\n", str);
}

void gen_initialize_va_list(int dst_struct_offset, int gp_offset, int fp_offset,
                            int reg_save_area_offset)
{
	printf("  movl $%d,  %d(%%rbp)\n", gp_offset, dst_struct_offset);
	printf("  movl $%d,  %d(%%rbp)\n", fp_offset, dst_struct_offset + 4);

	printf("  leaq 16(%%rbp), %%rax\n"
	       "  movq %%rax, %d(%%rbp)\n",
	       dst_struct_offset + 8);

	printf("  leaq %d(%%rbp), %%rax\n", reg_save_area_offset);
	printf("  movq %%rax, %d(%%rbp)\n", dst_struct_offset + 16);
}

/*

void debug_write(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

*/

void gen_write_stack_chk_guard_to_local(int offset)
{
#ifdef OSX
	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_write_to_local_8byte(offset);
	gen_discard();
#endif

#ifdef LINUX
	printf("  movq %%fs:40, %%rax\n");
	printf("  movq %%rax, %d(%%rbp)\n", offset);
#endif
}

void gen_epilogue_nbyte_with_stack_check(int n, int return_label_name,
                                         int checksum_offset,
                                         int failing_label_name)
{
#ifdef OSX
	gen_push_address_of_global("__stack_chk_guard");
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rax");
	printf("  cmpq %d(%%rbp), %%rax\n", checksum_offset);
#endif

#ifdef LINUX
	printf("  movq %d(%%rbp), %%rax\n", checksum_offset);
	printf("  cmpq %%fs:40, %%rax\n");
#endif

	printf("  jne .L%d\n", failing_label_name);
	gen_epilogue_nbyte(n, return_label_name);
	printf(".L%d:\n", failing_label_name);
	printf("  call " PREFIX "__stack_chk_fail\n");
}

int main()
{
	int fmt = -232;
	int reg_save_area = -192;
	int stack_check = -200;
	int ap = -224;

	gen_prologue(304, "debug_write");

	gen_write_register_to_local_8byte("rdi", fmt); /* fmt */
	gen_store_regs_to_local(reg_save_area, 1, "LBB0_2"); /* va_start(ap, fmt) */
	gen_write_stack_chk_guard_to_local(stack_check);
	gen_initialize_va_list(ap, 8, 0x30,
	                       reg_save_area); /* va_list ap; va_start(ap, fmt) */

	gen_push_address_of_global(
#ifdef OSX
		"__stderrp"
#endif
#ifdef LINUX
		"stderr"
#endif
		);
	gen_peek_and_dereference_nbyte(8);
	gen_pop_to_reg_8byte("rdi"); /* %rdi <- __stderrp */

	gen_push_address_of_local(ap);
	gen_pop_to_reg_8byte("rdx"); /* %rdx <- ap */

	gen_write_local_to_register_8byte(fmt, "rsi"); /* %rsi <- fmt */

	puts("  call " PREFIX "vfprintf");

	gen_initialize_va_list(ap, 8, 48, reg_save_area);
	
	gen_push_address_of_local(ap);
	gen_pop_to_reg_8byte("rsi"); /* %rsi <- ap */

	gen_write_local_to_register_8byte(fmt, "rdi"); /* %rdi <- fmt */

	puts("  call " PREFIX "vprintf");
	gen_push_int(123);
	gen_epilogue_nbyte_with_stack_check(4, 5421, stack_check, 6);
}
