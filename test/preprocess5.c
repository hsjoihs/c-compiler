#define A

int main()
{
	return
#ifndef A
	1
#endif
#ifdef A
	0
#endif
	;
}

