#include "../print_x86_64.h"
#include "../print_x86_64_unofficial.h"
#include <stdio.h>
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

int main()
{
	int fmt = -232;
	int reg_save_area = -192;
	int stack_check = -200;
	int ap = -224;

	gen_prologue(304, "debug_write");

	gen_write_register_to_local_8byte("rdi", fmt);       /* fmt */
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

	gen_push_from_local_8byte(fmt);
	gen_pop_to_reg_8byte("rsi"); /* %rsi <- fmt */

	puts("  call " PREFIX "vfprintf");

	gen_initialize_va_list(ap, 8, 48, reg_save_area);

	gen_push_address_of_local(ap);
	gen_pop_to_reg_8byte("rsi"); /* %rsi <- ap */

	gen_push_from_local_8byte(fmt);
	gen_pop_to_reg_8byte("rdi"); /* %rdi <- fmt */

	puts("  call " PREFIX "vprintf");
	gen_push_int(123);
	gen_epilogue_nbyte_with_stack_check(4, 5421, stack_check, 6);
}
