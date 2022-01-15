#include "Stack.h"

struct hashNode {
    int key;
    struct Stack* stack;
    struct hashNode* next;
};

struct Hashmap {
    int size;
    struct hashNode** list;
};

struct Hashmap* createTable(const int x, const int y);
int inline hashCode(const struct Hashmap* restrict t, const int key);
void insertStackToNode(struct Hashmap* restrict t, const int key, const int size);
struct Stack* hashGet(const struct Hashmap* t, const int key);
int getX(const struct Hashmap* board);
int getY(const struct Hashmap* board);
bool addMove(const struct Hashmap* board, const int column, const int* restrict tok);
void** getToken(const struct Hashmap* restrict board, const int x, const int y);
bool columnIsFull(const struct Hashmap* board, const int column);
bool isBoardFull(const struct Hashmap* restrict board, const int x);
void freeHashmap(struct Hashmap* restrict h);