#include <limits.h>
#include "common/data.h"

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

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

//struct Hashmap* copyBoard(const struct Hashmap* restrict board, const int x, const int y);
int count(const int* restrict window, int token);
void evaluateWindow(const int* restrict window, int* restrict score);
void getScore(const struct Hashmap* restrict board, const int* restrict centres, const int x, const int y, int* restrict finalScore);
GameStatus isGameOver(const struct Hashmap* restrict board, const int row, const int column);
int inline safeWinScore(const int scale, const int depth, const int maxDepth);
struct AIMove* minimax(const struct Hashmap* restrict board, const int x, const int y, const int column, const int* restrict centres, const int player, const int depth, const int maxDepth, int alpha, int beta);
void AIMakeMove(const struct Hashmap* restrict board, int* restrict column, const int* restrict centres, const int depth);
