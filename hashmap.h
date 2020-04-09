// source - http://www.kaushikbaruah.com/posts/data-structure-in-c-hashmap/

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

struct node {
    int key;
    struct stack* val;
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
    for (int i = 0; i < size; i++)
        t->list[i] = NULL; // this is where we will define the columns as stacks?
    return t;
}
int hashCode(struct table* t, int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}
void insert(struct table* t, int key, char val) {
    int pos = hashCode(t, key);
    struct node* list = t->list[pos];
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    struct node* temp = list;
    while (temp) {
        if (temp->key == key) {
            //temp->val = val;
            push(temp->val, val);
            return;
        }
        temp = temp->next;
    }
    newNode->key = key;
    //newNode->val = val;
    push(newNode->val, val);
    newNode->next = list;
    t->list[pos] = newNode;
}
struct stack* lookup(struct table* t, int key) {
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