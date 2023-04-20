#include "functions.h"

#define FAILED_MESSAGE "\n(!) failed to load the save correctly\n"
#define NOT_FOUND_MESSAGE "\n(!) file to load from was not found: 'save.bin'\n"
#define NONE_EXISTING_MESSAGE "\n(!) there is no existing save\n"

char* cancelLoad(struct Hashmap* restrict board, struct Hashmap* restrict history, struct Settings* restrict settings, FILE* file);
bool saveGame(const struct Hashmap* restrict board, const struct Hashmap* restrict history, const struct Settings* restrict settings, const bool turn, const bool traversing);
char* loadGame(struct Hashmap** restrict board, struct Hashmap** restrict history, struct Settings* restrict settings, bool* restrict turn, bool* restrict traversing);
