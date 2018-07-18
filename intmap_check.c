#include "map.h"
#include <assert.h>
#include <stdio.h>

int main()
{
	struct map map_ = init_int_map();
	assert(!isElem(map_, "ffjkl"));

	int v = 3;
	int w = 12;
	int x = 45;

	insert(&map_, "abc", &v);
	assert(3 == *(int *)lookup(map_, "kabc" + 1));
	assert(3 == *(int *)lookup(map_, "QQabc" + 2));
	insert(&map_, "abc", &w);
	assert(12 == *(int *)lookup(map_, "QQabc" + 2));
	assert(12 == *(int *)lookup(map_, "kabc" + 1));
	insert(&map_, "pqrs" + 1, &x);
	assert(45 == *(int *)lookup(map_, "QQqrs" + 2));
	assert(45 == *(int *)lookup(map_, "kqrs" + 1));
	deletion(&map_, "abc");
	assert(!isElem(map_, "abc"));
	assert(!isElem(map_, "ffjkl"));
	assert(45 == *(int *)lookup(map_, "QQqrs" + 2));
	assert(45 == *(int *)lookup(map_, "kqrs" + 1));

	fprintf(stderr, "\033[32mPASS\033[m\n");
	return 0;
}