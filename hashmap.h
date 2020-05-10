// source - http://www.kaushikbaruah.com/posts/data-structure-in-c-hashmap/

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

struct node {
    int key;
    struct stack* stack;
    struct node* next;
};
struct hashmap {
    int size;
    struct node** list;
};
void insertStackToNode(struct hashmap* t, int key, int size);
struct hashmap* createTable(int x, int y) {
    struct hashmap* t = (struct hashmap*)malloc(sizeof(struct hashmap));
    t->size = x;
    t->list = (struct node**)malloc(sizeof(struct node*) * x);
    //printf("\n\n! DEV:\n - nodes to build: %d\n - nodes built: ", x);
    for (int i = 0; i < x; i++)
        insertStackToNode(t, i, y);
    return t;
}
int hashCode(struct hashmap* t, int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}
void insertStackToNode(struct hashmap* t, int key, int size) { // would it be better to recursively call this method on the next node? Rather than iterating it for the amount of stacks we expect
    if (key >= t->size)
        return; // return boolean to determine success
    int pos = hashCode(t, key);
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->key = key;
    newNode->stack = createStack(size);
    if (pos + 1 <= t->size)
        newNode->next = t->list[pos + 1];
    else
        newNode->next = NULL;
    t->list[pos] = newNode;
}
struct stack* hashGet(struct hashmap* t, int key) {
    int pos = hashCode(t, key);
    struct node* list = t->list[pos];
    struct node* temp = list;
    while (temp) {
        if (temp->key == key)
            return temp->stack;
        temp = temp->next;
    }
    return NULL;
}
int getY(struct hashmap* board) {
    return hashGet(board, 0)->size;
}
bool addMove(struct hashmap* board, int column, int tok) {
    return push(hashGet(board, column - 1), tok);
}
int getToken(struct hashmap* board, int x, int y) {
    return stackGet(hashGet(board, x), y);
}