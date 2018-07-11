#include "intmap.h"
#include <string.h>

/* it overrides; it does not overwrite. */
void insert(struct int_map *map_ptr, const char *key, int value)
{
	struct charptANDint a;
	a.ptr = key;
	a.value = value;
	push_vector_charptANDint(map_ptr, a);
}

/* returns garbage (0xCCCCCCCC == GARBAGE_INT) if not found. */
int lookup(const struct int_map map, const char *key)
{
	for (int i=(map.length)-1; i>=0; --i) {
		if (strcmp(map.vector[i].ptr, key) == 0) {
			return map.vector[i].value;
		}
	}
	return GARBAGE_INT;
}

void deletion(struct int_map *map_ptr, const char *key)
{
	insert(map_ptr, key, GARBAGE_INT);
}

struct int_map init_int_map(void)
{
	return init_vector_charptANDint(0);
}

