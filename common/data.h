#include "functions.h"

#define FAILED_MESSAGE "\n(!) failed to load the save correctly\n"
#define NOT_FOUND_MESSAGE "\n(!) file to load from was not found: 'save.bin'\n"
#define NONE_EXISTING_MESSAGE "\n(!) there is no existing save\n"

char* cancelLoad(struct Hashmap* board, struct Hashmap* history, struct Settings* settings, FILE* file);
bool saveGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool turn, bool traversing);
char* loadGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool* turn, bool* traversing);
