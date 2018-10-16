#include <stdio.h>
#include <stdlib.h>

int GLOBAL_VAR = 3;

int ptrdiff(void *p1, void *p2) { return (char *)p1 - (char *)p2; }

int *alloc4(int a, int b, int c, int d)
{
	int *p = calloc(4, sizeof(int));
	p[0] = a;
	p[1] = b;
	p[2] = c;
	p[3] = d;
	return p;
}

int foobar(const char *str, int i, int j)
{
	printf(str, i, j);
	return 0;
}

struct INT_CHAR_CHAR_INT {
	int a;
	char c;
	char d;
	int b;
};

struct INT_CHAR_CHAR_INT *get_struct_pointer(int a, int b)
{
	struct INT_CHAR_CHAR_INT *p = calloc(1, sizeof(struct INT_CHAR_CHAR_INT));
	p->a = a;
	p->b = b;
	return p;
}

int add_3(int a){return 3 + a;}

void *return_fp(void){return add_3;}

int call_fp(void* q){int (*p)(int) = q;return (*p)(171);}
