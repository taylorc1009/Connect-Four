#include <stdio.h>
#include <stdlib.h>

#define NUL '\0' // used to "nullify" a char

typedef enum { false, true } bool;

struct stackNode {
	int player;
	struct stackNode* prev;
	struct stackNode* next;
};
struct stack {
	struct stackNode** list;
	int top;
	int size;
};
struct stack* createStack(int size) {
	struct stack* s = (struct stack*)malloc(sizeof(struct stack));
	s->top = -1;
	s->size = size;
	s->list = (struct stackNode**)malloc(sizeof(struct stackNode*) * size);
	for (int i = 0; i < size; i++)
		s->list[i] = NULL;
	return s;
}
bool push(struct stack* s, int player) {
	if (s->top == s->size - 1)
		return true;
	struct stackNode* newNode = (struct stackNode*)malloc(sizeof(struct stackNode));
	if (s->top != -1)
		newNode->prev = s->list[s->top];
	else
		newNode->prev = NULL;
	s->top++;
	newNode->player = player;
	newNode->next = NULL;
	s->list[s->top] = newNode;
	return false;
}
struct stackNode** pop(struct stack* s) {
	struct stackNode** data;
	if (s->top == -1) {
		printf(" Stack is empty \n");
		return NULL; // use this value during the undo to check for unsuccessful pops
	}
	data = &s->list[s->top];
	s->top--;
	return data;
}
int stackGet(struct stack* s, int index) {
	if (index < s->size && s->list[index] != NULL)
		return s->list[index]->player;
	else
		return 0;
}