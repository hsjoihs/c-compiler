#include <stdlib.h>
struct F {
	int k;
	int *a;
	int *b;
	char *c;
	char *d;
	int *e;
};

void baz(struct F *ptr)
{
	ptr->k = 3;
	int *q = calloc(3, sizeof(int));
	q[0] = 4;
	q[1] = 5;
	q[2] = 6;
	ptr->a = q;
	ptr->b = q + 1;
	ptr->e = q + 2;
}
