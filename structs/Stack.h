#include "../identifiers.h"

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

struct Stack* createStack(const int size);
bool push(struct Stack* restrict s, const void** restrict val);
bool pop(struct Stack* restrict s);
void* stackGet(const struct Stack* restrict s, const int index);
bool stackIsFull(const struct Stack* restrict s);
void resizeStack(struct Stack* restrict s, const int n);
void freeStack(struct Stack* restrict s);