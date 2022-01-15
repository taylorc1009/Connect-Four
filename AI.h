#include <limits.h>
#include "common/data.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

typedef enum {
	Ongoing = 0, //game is still going
	AIWin = 1,
	PlayerWin = 2,
	BoardFull = 3
} GameStatus;

struct AIMove {
	int column;
	int score;
	GameStatus gameStatus;
};

struct Hashmap* copyBoard(struct Hashmap* board, int x, int y);
int count(const int* restrict window, int token);
void evaluateWindow(const int* restrict window, int* score);
void getScore(struct Hashmap* board, int* centres, int x, int y, int* finalScore);
GameStatus isGameOver(struct Hashmap* board, int row, int column);
int safeWinScore(int scale, int depth, int maxDepth);
struct AIMove* minimax(struct Hashmap* board, int x, int y, int column, int* centres, int player, int depth, int maxDepth, int alpha, int beta);
void AIMakeMove(struct Hashmap* board, int* column, int* centres, int depth);
