#include "map.h"
#include "std.h"

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
void insert(struct Map2 **ptr, const char *key, void *value)
{
	struct _mapchip a;
	a.ptr = key;
	a.value = value;

	if ((*ptr)->_alloc < (*ptr)->_length + 1) {

		(*ptr)->_vec =
		    realloc((*ptr)->_vec, (*ptr)->_alloc * 2 * sizeof(struct _mapchip));

		(*ptr)->_alloc *= 2;
	}

	(*ptr)->_vec[(*ptr)->_length] = a;
	++((*ptr)->_length);
}

void *lookup(const struct Map2 *m, const char *key)
{
	for (int i = (m->_length) - 1; i >= 0; --i) {
		if (strcmp(m->_vec[i].ptr, key) == 0) {
			return m->_vec[i].value;
		}
	}
	return 0;
}

int isElem(const struct Map2 *m, const char *key)
{
	return lookup(m, key) != 0;
}

struct Map2 *init_map(void)
{
	struct Map2 *res;
	res = calloc(1, sizeof(struct Map2));
	res->_length = 0;
	res->_alloc = 256;
	res->_vec = calloc(res->_alloc, sizeof(struct _mapchip));
	return res;
}
