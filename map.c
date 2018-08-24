#include "map.h"
#include "std.h"
#include <stdio.h>

struct Map2 {
	int _length;
	int _alloc;
	struct _mapchip *_vec;
};

struct _mapchip {
	const char *ptr;
	void *value;
};

/* it overrides; it does not overwrite. */
void insert(struct Map *ptr, const char *key, void *value)
{
	struct _mapchip a;
	a.ptr = key;
	a.value = value;

	if (ptr->internal->_alloc < ptr->internal->_length + 1) {

		ptr->internal->_vec =
		    realloc(ptr->internal->_vec,
		            ptr->internal->_alloc * 2 * sizeof(struct _mapchip));

		ptr->internal->_alloc *= 2;
	}

	ptr->internal->_vec[ptr->internal->_length] = a;
	++(ptr->internal->_length);
}

void *lookup(const struct Map m, const char *key)
{
	for (int i = (m.internal->_length) - 1; i >= 0; --i) {
		if (strcmp(m.internal->_vec[i].ptr, key) == 0) {
			return m.internal->_vec[i].value;
		}
	}
	return 0;
}

int isElem(const struct Map m, const char *key) { return lookup(m, key) != 0; }

struct Map init_map(void)
{
	struct Map res;
	res.internal = calloc(1, sizeof(struct Map2));
	res.internal->_length = 0;
	res.internal->_alloc = 256;
	res.internal->_vec = calloc(res.internal->_alloc, sizeof(struct _mapchip));
	return res;
}

struct Map clone_map(const struct Map map)
{
	struct Map res;
	res.internal = calloc(1, sizeof(struct Map2));
	res.internal->_length = map.internal->_length;
	res.internal->_alloc = map.internal->_alloc;
	res.internal->_vec = calloc(res.internal->_alloc, sizeof(struct _mapchip));
	for (int i = 0; i < res.internal->_length; i++) {
		res.internal->_vec[i] = map.internal->_vec[i];
	}
	return res;
}
