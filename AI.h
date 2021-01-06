#include <math.h>
#include "Hashmap.h"

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