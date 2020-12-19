//source - http://www.kaushikbaruah.com/posts/data-structure-in-c-Hashmap/

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

void insertStackToNode(struct Hashmap* t, int key, int size);

struct Hashmap* createTable(int x, int y) {
    struct Hashmap* t = (struct Hashmap*)malloc(sizeof(struct Hashmap));
    t->size = x;
    t->list = (struct hashNode**)malloc(sizeof(struct hashNode*) * x);

    for (int i = 0; i < x; i++)
        insertStackToNode(t, i, y);

    return t;
}
int hashCode(struct Hashmap* t, int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}

void insertStackToNode(struct Hashmap* t, int key, int size) { //would it be better to recursively call this method on the next hashNode? Rather than iterating it for the amount of stacks we expect
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
struct Stack* hashGet(struct Hashmap* t, int key) {
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

int getX(struct Hashmap* board) {
    return board->size;
}

int getY(struct Hashmap* board) {
    return hashGet(board, 0)->size;
}

bool addMove(struct Hashmap* board, int column, int tok) {
    return push(hashGet(board, column), tok);
}

int getToken(struct Hashmap* board, int x, int y) {
    return stackGet(hashGet(board, x), y);
}

bool isBoardFull(struct Hashmap* board, int x) {
    for(int i = 0; i < x; i++)
        if(!stackIsFull(hashGet(board, i)))
            return false;
    return true;
}