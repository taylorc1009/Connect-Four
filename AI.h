#include <math.h>
#include "Hashmap.h"

#define EMPTY_SLOT 0

#define ARRAY_LENGTH(x) ((int)sizeof(x) / sizeof((x)[0])) //we use this because we cannot calculate the size of a dynamic array; the compiler can never know its size

struct AIMove {
	int column;
	int score;
	bool gameOver;
};

struct AIMove* minimax(struct Hashmap* board, int x, int y, int column, int* centres, int player, int depth, int maxDepth, int alpha, int beta);
void getScore(struct Hashmap* board, int* centres, int x, int y, int* finalScore);
void AIMakeMove(struct Hashmap* board, int* column, int* centres, int depth);
struct Hashmap* copyBoard(struct Hashmap* board, int x, int y);
bool isGameOver(struct Hashmap* board, int row, int column);
struct AIMove* minimax(struct Hashmap* board, int x, int y, int column, int* centres, int player, int depth, int maxDepth, int alpha, int beta);
void evaluateWindow(int* window, int size, int* score);
void getScore(struct Hashmap* board, int* centres, int x, int y, int* finalScore);
int count(int* list, int n, int tok);