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

struct Stack* createStack(int size);
bool push(struct Stack* s, void** val);
bool pop(struct Stack* s);
void* stackGet(struct Stack* s, int index);
bool stackIsFull(struct Stack* s);
void resizeStack(struct Stack* s, int n);
void freeStack(struct Stack* s);