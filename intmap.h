#include "vector.h"

#define int_map vector_charptANDint

void insert(struct int_map *map, const char *key, int value);
int lookup(const struct int_map *map, const char *key);
void deletion(struct int_map *map_ptr, const char *key);
