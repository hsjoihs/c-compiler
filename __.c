#include "__.h"   
#include "map.h"   

INT main()
{
	struct Map2 *m = init_map();
	int a = 3;
	int *p = &a;
	insert(m, "3", p);
	insert(m, "three", p);
	int *q = lookup(m, "3");
	return *q - 3;
}
