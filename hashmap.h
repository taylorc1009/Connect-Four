#include <stdio.h>
#include <stdlib.h>
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

struct Hashmap* createTable(int x, int y);
int hashCode(struct Hashmap* t, int key);
void insertStackToNode(struct Hashmap* t, int key, int size);
struct Stack* hashGet(struct Hashmap* t, int key);
int getX(struct Hashmap* board);
int getY(struct Hashmap* board);
bool addMove(struct Hashmap* board, int column, int* tok);
void** getToken(struct Hashmap* board, int x, int y);
bool isBoardFull(struct Hashmap* board, int x);
void freeHashmap(struct Hashmap* h);