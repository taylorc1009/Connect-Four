#include <stdio.h>
#include <stdlib.h>

#define NUL '\0' // used to "nullify" a char

typedef enum { false, true } bool;

struct stackNode {
	char val;
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
bool push(struct stack* s, char val) { // return boolean to determine push success
	if (s->top == s->size - 1)
		return true;
	struct stackNode* newNode = (struct stackNode*)malloc(sizeof(struct stackNode));
	if (s->top != -1)
		newNode->prev = s->list[s->top];
	else
		newNode->prev = NULL;
	s->top++;
	newNode->val = val;
	newNode->next = NULL;
	s->list[s->top] = newNode;
	return false;
}
struct stackNode** pop(struct stack* s) {
	struct stackNode** data;
	if (s->top == -1) {
		printf(" Stack is empty \n");
		return NULL;
	}
	data = &s->list[s->top];
	s->top--;
	// should the data be freed here? No, we still need this portion of memory to be reserved in case new data is added here later
	return data;
}
char stackGet(struct stack* s, int index) {
	if (index < s->size && s->list[index] != NULL)
		return s->list[index]->val;
	else
		return NUL;
}