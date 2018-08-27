struct Map2;

void insert(struct Map2 *map, const char *key, void *value);
void *lookup(const struct Map2 *map, const char *key);
struct Map2 *init_map(void);
int isElem(const struct Map2 *map, const char *key);
