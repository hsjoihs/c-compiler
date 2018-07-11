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

/* returns garbage (0xCCCCCCCC) if not found. */
int lookup(const struct int_map *map_ptr, const char *key)
{
	for (int i=(map_ptr->length)-1; i>=0; --i) {
		if (strcmp(map_ptr->vector[i].ptr, key) == 0) {
			return map_ptr->vector[i].value;
		}
	}
	return 0xCCCCCCCC;
}

void deletion(struct int_map *map_ptr, const char *key)
{
	insert(map_ptr, key, 0xCCCCCCCC);
}
