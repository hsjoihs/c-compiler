#include <stdio.h>
#include "lexer.h"
#include "print_assembly.h"
#include "vector.h"

int main()
{
	/* 
		a = b = 9, a = 41*3, 55 - (b = 4) + a 
	*/

	struct vector_Int offsets = init_vector_Int(0);
	struct Int a_addr = {-4};
	struct Int b_addr = {-8};
	
	print_header(8);

	push_vector_Int(&offsets, a_addr); /* a as lvalue */
	push_vector_Int(&offsets, b_addr); /* b as lvalue */
	push_int(9); /* 9 */
	write_to_local(pop_vector_Int(&offsets).i);/* = */
	write_to_local(pop_vector_Int(&offsets).i);/* = */

	push_vector_Int(&offsets, a_addr); /* a as lvalue */
	push_int(41); /* 41 */
	push_int(3); /* 3 */
	mul_ints(); /* * */
	write_to_local(pop_vector_Int(&offsets).i);/* = */
	op_ints("movl"); /* , */

	push_int(55); /* 55 */
	push_vector_Int(&offsets, b_addr); /* b as lvalue */
	push_int(4); /* 4 */
	write_to_local(pop_vector_Int(&offsets).i);/* = */
	op_ints("subl"); /* - */
	push_from_local(a_addr.i); /* a as rvalue */
	op_ints("addl"); /* + */

	op_ints("movl"); /* , */
	print_footer(8);
	return 0;
}
