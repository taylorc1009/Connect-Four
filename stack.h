#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool; //we use a typedef instead of '#include<stdbool.h>' as "bool" is not a key-word to the compiler, so including it would give errors

struct stackNode {
	void* val; //a pointer with an unspecified type - allows us to point to any kind of value
	struct stackNode* prev;
	struct stackNode* next;
};

struct Stack {
	struct stackNode** list;
	int top;
	int size;
};

struct Stack* createStack(int size) {
	struct Stack* s = (struct Stack*)malloc(sizeof(struct Stack));

	s->top = -1;
	s->size = size;
	s->list = (struct stackNode**)malloc(sizeof(struct stackNode*) * size);

	for (int i = 0; i < size; i++)
		s->list[i] = NULL;

	return s;
}

bool push(struct Stack* s, void** val) {
	if (stackIsFull(s)) //if the stack is full
		return true;

	struct stackNode* newNode = (struct stackNode*)malloc(sizeof(struct stackNode));

	if (s->top != -1) //if the stack is not empty
		newNode->prev = s->list[s->top];
	else
		newNode->prev = NULL;
	s->top++;

	newNode->val = *val;
	newNode->next = NULL;
	s->list[s->top] = newNode;

	return false;
}

bool pop(struct Stack* s) {
	if (s->top == -1)
		return false;

	free(s->list[s->top]->val);
	free(s->list[s->top]);
	s->list[s->top] = NULL;
	s->top--;

	return true;
}

void* stackGet(struct Stack* s, int index) {
	if (index < s->size && s->list[index] != NULL)
		return s->list[index]->val;
	else
		return NULL;
}

bool stackIsFull(struct Stack* s) {
	return s->top == s->size - 1;
}

void resizeStack(struct Stack* s, int n) {
	s->list = (struct stackNode**)realloc(s->list, (sizeof(struct stackNode) * (s->size + n)));
	n > 0 ? s->size++ : s->size--;
}

void freeStack(struct Stack* s) {
	for (int i = 0; i < s->size; i++) {
		if (i <= s->top)
			free(s->list[i]->val);
		free(s->list[i]);
	}
}