#pragma once

struct map {
	int _length;
	int _alloc;
	struct _mapchip* _vec;
};

void insert(struct map *map_ptr, const char *key, void *value);
void *lookup(const struct map map, const char *key);
void deletion(struct map *map_ptr, const char *key);
struct map init_int_map(void);
int isElem(const struct map map, const char *key);
