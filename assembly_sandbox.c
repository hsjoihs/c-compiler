#include "print_x86_64.h"
#include <stdio.h>

/*
int main(){int a[1][2];int (*p)[2];p = a;int *q;q = *(p+1); *q=174; return
**(a+1);}
*/

int main()
{
	gen_prologue(32, "main");

	/* p = a; */
	gen_push_address_of_local(-24); /* &a[0] */
	gen_write_to_local_8byte(-8);   /* p= */
	gen_discard();                  /* ; */

	/* q = *(p+1); */
	gen_push_from_local_8byte(-8);
	gen_push_int(8);
	gen_op_8byte("addq"); /* p+1 :: pointer to array(length 2) */

	/* pointer to array does not change its value when dereferenced */
	gen_write_to_local_8byte(-16);
	gen_discard();

	gen_push_from_local_8byte(-16); /* q */
	gen_push_int(174);              /* 174 */
	gen_assign_4byte();             /* *q = 174; */
	gen_discard();

	gen_push_from_local(-16);
	gen_epilogue(12345);
}
