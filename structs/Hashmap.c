#include "Hashmap.h"

struct Hashmap* createTable(const int x, const int y) {
    struct Hashmap* t = (struct Hashmap*)malloc(sizeof(struct Hashmap));
    t->size = x;
    t->list = (struct hashNode**)malloc(sizeof(struct hashNode*) * x);

    for (int i = 0; i < x; i++)
        insertStackToNode(t, i, y);

    return t;
}

int inline hashCode(const struct Hashmap* restrict t, const int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}

void insertStackToNode(struct Hashmap* restrict t, const int key, const int size) { //would it be better to recursively call this method on the next hashNode? Rather than iterating it for the amount of stacks we expect
    if (key >= t->size)
        return; //return boolean to determine success

    int pos = hashCode(t, key);

    struct hashNode* newNode = (struct hashNode*)malloc(sizeof(struct hashNode));
    newNode->key = key;
    newNode->stack = createStack(size);

    if (pos + 1 <= t->size)
        newNode->next = t->list[pos + 1];
    else
        newNode->next = NULL;

    t->list[pos] = newNode;
}

struct Stack* hashGet(const struct Hashmap* restrict t, const int key) {
    int pos = hashCode(t, key);

    struct hashNode* list = t->list[pos];
    struct hashNode* temp = list;

    while (temp) {
        if (temp->key == key)
            return temp->stack;
        temp = temp->next;
    }

    return NULL;
}

int getX(const struct Hashmap* board) {
    return board->size;
}

int getY(const struct Hashmap* board) {
    return hashGet(board, 0)->size;
}

bool addMove(const struct Hashmap* board, const int column, const int* restrict tok) {
    return push(hashGet(board, column), (void**)&tok);
}

void** getToken(const struct Hashmap* restrict board, const int x, const int y) {
    void** val = stackGet(hashGet(board, x), y);
    if (val == NULL) {
        static void* nul = EMPTY_SLOT;
        return (&nul);
    }
    return val;
}

bool columnIsFull(const struct Hashmap* board, const int column) {
    return stackIsFull(hashGet(board, column));
}

bool isBoardFull(const struct Hashmap* restrict board, const int x) {
    for (int i = 0; i < x; i++)
        if (!columnIsFull(board, i))
            return false;
    return true;
}

void freeHashmap(struct Hashmap* restrict h) { //use this to clear the entire board data from memory
    for (int i = 0; i < h->size; i++) {
        freeStack(h->list[i]->stack);
        free(h->list[i]->stack);
        free(h->list[i]);
    }
    free(h->list);
    free(h);
}