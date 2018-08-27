struct Map2;

#define Map Map2 *

void insert(struct Map *map_ptr, const char *key, void *value);
void *lookup(const struct Map map, const char *key);
struct Map init_map(void);
int isElem(const struct Map map, const char *key);
