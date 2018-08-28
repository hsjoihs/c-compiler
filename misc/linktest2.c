#include <stdio.h>
#include <assert.h>
struct F {int k; int *a; int *b; char *c; char *d; int *e;};
struct F baz(void);

int main()
{
	struct F f = baz();
	assert(f.k == 3);
	assert(*f.a == 4);
	assert(*f.b == 5);
	assert(*f.e == 6);
	printf("successful\n");
	return 0;
}

