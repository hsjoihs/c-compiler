#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"

int main()
{
	int offset;
	/* 
		b = 9, a = 41*3, 55 - (b = 4) + a 
	*/
	print_header(8);

	//offset = -4; /* a */
	offset = -8; /* b */
	push_int(9); /* 9 */
	write_to_local(offset);/* = */
	//write_to_local(offset);/* = */

	offset = -4; /* a */
	push_int(41); /* 41 */
	push_int(3); /* 3 */
	mul_ints(); /* * */
	write_to_local(offset); /* = */
	op_ints("movl"); /* , */

	push_int(55); /* 55 */
	offset = -8; /* b */
	push_int(4); /* 4 */
	write_to_local(offset);/* = */
	op_ints("subl"); /* - */
	push_from_local(-4); /* a */
	op_ints("addl"); /* + */

	op_ints("movl"); /* , */
	print_footer(8);
	return 0;
}
