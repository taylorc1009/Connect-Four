// source - http://www.kaushikbaruah.com/posts/data-structure-in-c-hashmap/

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

struct node {
    int key;
    struct stack* stack;
    struct node* next;
};
struct table {
    int size;
    struct node** list;
};
void insertStackToNode(struct table* t, int key, int size);
struct table* createTable(int x, int y) {
    struct table* t = (struct table*)malloc(sizeof(struct table));
    t->size = x;
    t->list = (struct node**)malloc(sizeof(struct node*) * x);
    //printf("\n\n! DEV:\n - nodes to build: %d\n - nodes built: ", x);
    for (int i = 0; i < x; i++) {
        //printf("%d", i + 1);
        /*t->list[i]->key = i;
        t->list[i]->stack = createStack(y);
        t->list[i]-*/
        insertStackToNode(t, i, y);
    }
    return t;
}
int hashCode(struct table* t, int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}
void insertStackToNode(struct table* t, int key, int size) { // would it be better to recursively call this method on the next node? Rather than iterating it with the amount of stacks we expect
    if (key >= t->size)
        return; // return boolean to determine success
    int pos = hashCode(t, key);
    //struct node* list = t->list[pos];
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    /*struct node* temp = list;
    while (temp) {
        if (temp->key == key) {
            temp->val = val;
            //push(temp->stack, val);
            return;
        }
        temp = temp->next;
    }*/
    newNode->key = key;
    newNode->stack = createStack(size);
    //push(newNode->stack, val);
    if (pos + 1 <= t->size)
        newNode->next = t->list[pos + 1];
    else
        newNode->next = NULL;
    t->list[pos] = newNode;
}
struct stack* hashGet(struct table* t, int key) {
    int pos = hashCode(t, key);
    struct node* list = t->list[pos];
    struct node* temp = list;
    while (temp) {
        //printf("1");
        if (temp->key == key) {
            return temp->stack;
        }
        temp = temp->next;
    }
    return NULL;
}

/*#include <stdio.h>
#include <stdlib.h>

struct node {
    int key;
    int val;
    struct node* next;
};
struct table {
    int size;
    struct node** list;
};
struct table* createTable(int size) {
    struct table* t = (struct table*)malloc(sizeof(struct table));
    t->size = size;
    t->list = (struct node**)malloc(sizeof(struct node*) * size);
    int i; // move inside for loop declaration
    for (i = 0; i < size; i++)
        t->list[i] = NULL; // this is where we will define the columns as stacks?
    return t;
}
int hashCode(struct table* t, int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}
void insert(struct table* t, int key, int val) {
    int pos = hashCode(t, key);
    struct node* list = t->list[pos];
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    struct node* temp = list;
    while (temp) {
        if (temp->key == key) {
            temp->val = val;
            return;
        }
        temp = temp->next;
    }
    newNode->key = key;
    newNode->val = val;
    newNode->next = list;
    t->list[pos] = newNode;
}
int lookup(struct table* t, int key) {
    int pos = hashCode(t, key);
    struct node* list = t->list[pos];
    struct node* temp = list;
    while (temp) {
        if (temp->key == key) {
            return temp->val;
        }
        temp = temp->next;
    }
    return -1;
}*/