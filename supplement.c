#include <stdio.h>
#include <stdlib.h>
int always87() { return 87; }
int always8() { return 8; }
int add(int x, int y) { return x + y; }
int subtract(int x, int y) { return x - y; }
int *alloc4(int a, int b, int c, int d)
{
	int *p = calloc(4, sizeof(int));
	p[0] = a;
	p[1] = b;
	p[2] = c;
	p[3] = d;
	return p;
}
int qwerty(char a, char b)
{
	int d;
	d = 3;
	char c;
	c = a + d;
	return c * 4;
}
int qwer(char a, char b)
{
	int d;
	d = 3;
	char c;
	c = a + d;
	return c * b;
}
int foobar(const char *str, int i, int j)
{
	printf(str, i, j);
	return 0;
}

int ptrdiff(void *p1, void *p2) { return (char *)p1 - (char *)p2; }
