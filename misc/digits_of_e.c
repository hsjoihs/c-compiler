#include "std_io.h"
// Taken from http://www.uranus.dti.ne.jp/~n-sasaki/toyprogs.html
int *p;
int w[2048];
int n;
int x;
int main()
{
	printf("2.");
	n = 825;
	for (p = w; ++p - w >> 11 ? p = w, x = n-- : 1;)
		n ? x = x % n * 10 + *p, *p = x / n : printf("%d", *p);
	return 0;
}