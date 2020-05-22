#include <math.h>
#include "hashmap.h"

#define PLAYER_1_TOKEN 1
#define PLAYER_2_TOKEN 2
#define EMPTY_SLOT 0

#define MINIMAX_DEPTH 3

#define ARRAY_LENGTH(x) ((int)sizeof(x) / sizeof((x)[0]))

struct Move {
	int column;
	int score;
};

void pickBestMove(struct hashmap* board, int x, int y, int* column);
void freeBoard(struct hashmap* board);
struct Move* minimax(struct hashmap* board, int x, int y, int column, int* centres, int player, int depth);
void getScore(struct hashmap* board, int* centres, int x, int y, int* finalScore);

void delay(int numOfSeconds) {
	int milliSeconds = 1000 * numOfSeconds;
	clock_t startTime = clock();
	while (clock() < startTime + milliSeconds);
}

void AIMakeMove(struct hashmap* board, int* column, int* centres) {
	int x = getX(board), y = getY(board);
	//pickBestMove(board, x, y, column);
	//struct Move* move = (struct Move*)malloc(sizeof(struct Move));
	struct Move* move = minimax(board, x, y, -1, centres, PLAYER_2_TOKEN, MINIMAX_DEPTH);
	*column = move->column + 1;
	printf("\nfinal score & column = %d, %d", move->score, move->column);
	free(move);
	//printf("\npicked column: %d", *column);
	//delay(3);
}

bool isGameOver(struct hashmap* board, int row, int column, int p) {
	//printf("\ncheckWin = %s, isBoardFull = %s >> row: %d, column: %d, tok: %d\n", checkWin(row, column, board, p) ? "true" : "false", isBoardFull(board, getX(board)) ? "true" : "false", row, column, p);
	return checkWin(row, column, board, p) || isBoardFull(board, getX(board));
}

struct Move* minimax(struct hashmap* board, int x, int y, int column, int* centres, int player, int depth) {
	struct Move* move = (struct Move*)malloc(sizeof(struct Move));
	move->column = 0;
	//printf("\n%d. (%d, %d), col: %d, tok: %d", depth, x, y, column, player);
	if (column != -1) {
		int row = hashGet(board, column)->top + 1;
		bool gameOver = isGameOver(board, row, column, player);
		printf(" >> gameOver? %s", gameOver ? "true" : "false");

		if (depth == 0 || gameOver) {
			if (gameOver) {
				if (checkWin(row, column, board, PLAYER_2_TOKEN))
					move->score = 10000; // bot wins in this instance
				else if (checkWin(row, column, board, PLAYER_1_TOKEN))
					move->score = -10000; // player wins in this instance
				else
					move->score = 0; // board is full
			}
			else
				//return getScore(board, centres, row, column, score);
				getScore(board, centres, x, y, &(move)->score);
			return move;
		}
	}
	if (player == PLAYER_2_TOKEN) { // maximizing player
		move->score = -2147483648;
		for (int i = 0; i < x; i++) {
			if (!stackIsFull(hashGet(board, i))) {
				struct hashmap* temp = createTable(x, y);
				for (int j = 0; j < x; j++) {
					for (int k = 0; k < y; k++) {
						int tok = getToken(board, j, k);
						if (tok != 0)
							addMove(temp, j, tok);
					}
				}
				addMove(temp, i, PLAYER_2_TOKEN);
				struct Move* newMove = minimax(temp, x, y, i, centres, PLAYER_1_TOKEN, depth - 1);
				//printf("\nmove = { %d, %d }, newMove = { %d, %d }", move->score, move->column, newMove->score, newMove->column);
				if (newMove->score > move->score) {
					move->score = newMove->score;
					move->column = i;
					//printf(" >> move->score changed = %d, column = %d", move->score, move->column);
				}
				freeBoard(temp);
				free(newMove);
			}
		}
		//printf("\n");
		return move;
	}
	else { // minimizing player
		move->score = 2147483647;
		for (int i = 0; i < x; i++) {
			if (!stackIsFull(hashGet(board, i))) {
				struct hashmap* temp = createTable(x, y);
				for (int j = 0; j < x; j++) {
					for (int k = 0; k < y; k++) {
						int tok = getToken(board, j, k);
						if (tok != 0)
							addMove(temp, j, tok);
					}
				}
				addMove(temp, i, PLAYER_1_TOKEN);
				struct Move* newMove = minimax(temp, x, y, i, centres, PLAYER_2_TOKEN, depth - 1);
				//printf("\nmove = { %d, %d }, newMove = { %d, %d }", move->score, move->column, newMove->score, newMove->column);
				if (newMove->score < move->score) {
					move->score = newMove->score;
					move->column = i;
					//printf(" >> move->score changed = %d, column = %d", move->score, move->column);
				}
				freeBoard(temp);
				free(newMove);
			}
		}
		//printf("\n");
		return move;
	}
}

//void minimax(struct hashmap* board, int x, int y, int column, int* centres, int player, int depth, struct Move* move) {
//	printf("\n%d. (%d, %d), col: %d, tok: %d >> Move: score->%d column->%d", depth, x, y, column, player, move->score, move->column);
//	if (column != -1) {
//		int row = hashGet(board, column)->top + 1;
//		bool gameOver = isGameOver(board, row, column, player);
//		//printf(" >> gameOver? %s", gameOver ? "true" : "false");
//
//		if (depth == 0 || gameOver) {
//			if (gameOver) {
//				if (checkWin(row, column, board, PLAYER_2_TOKEN))
//					//return 100;
//					move->score = 10000;
//				else if (checkWin(row, column, board, PLAYER_1_TOKEN))
//					//return -100;
//					move->score = -10000;
//				else
//					//return 0;
//					move->score = 0;
//			}
//			else
//				//return getScore(board, centres, row, column, score);
//				getScore(board, centres, row, column, &(move)->score);
//			return;
//		}
//	}
//	if (player == PLAYER_2_TOKEN) { // maximizing player
//		struct Move* newMove = (struct Move*)malloc(sizeof(struct Move));
//		newMove->score = -2147483648;
//		newMove->column = 0;
//		for (int i = 0; i < x; i++) {
//			if (!stackIsFull(hashGet(board, i))) {
//				struct hashmap* temp = createTable(x, y);
//				for (int j = 0; j < x; j++) {
//					for (int k = 0; k < y; k++) {
//						int tok = getToken(board, j, k);
//						if (tok != 0)
//							addMove(temp, j, tok);
//					}
//				}
//				addMove(temp, i, PLAYER_2_TOKEN);
//				minimax(temp, x, y, i, centres, PLAYER_1_TOKEN, depth - 1, newMove);
//				freeBoard(temp);
//				printf("\nnewMove = %d, move = %d", newMove->score, move->score);
//				if (newMove->score > move->score) {
//					move->score = newMove->score;
//					printf("\nmove->score changed = %d", move->score);
//					move->column = i;
//				}
//			}
//		}
//		free(newMove);
//	}
//	else { // minimizing player
//		struct Move* newMove = (struct Move*)malloc(sizeof(struct Move));
//		newMove->score = 2147483647;
//		newMove->column = 0;
//		for (int i = 0; i < x; i++) {
//			if (!stackIsFull(hashGet(board, i))) {
//				struct hashmap* temp = createTable(x, y);
//				for (int j = 0; j < x; j++) {
//					for (int k = 0; k < y; k++) {
//						int tok = getToken(board, j, k);
//						if (tok != 0)
//							addMove(temp, j, tok);
//					}
//				}
//				addMove(temp, i, PLAYER_1_TOKEN);
//				minimax(temp, x, y, i, centres, PLAYER_2_TOKEN, depth - 1, newMove);
//				freeBoard(temp);
//				if (newMove->score < move->score) {
//					move->score = newMove->score;
//					printf("\nmove->score changed = %d", move->score);
//					move->column = i;
//				}
//			}
//		}
//		free(newMove);
//	}
//	printf("\n");
//}

void evaluateWindow(int* window, int size, int* score) {
	if (count(window, size, PLAYER_2_TOKEN) == 2 && count(window, size, EMPTY_SLOT) == 2)
		*score += 5;
	else if (count(window, size, PLAYER_2_TOKEN) == 3 && count(window, size, EMPTY_SLOT) == 1)
		*score += 10;
	else if (count(window, size, PLAYER_2_TOKEN) == 4)
		*score += 100;
	else if (count(window, size, PLAYER_1_TOKEN) == 3 && count(window, size, EMPTY_SLOT) == 1)
		*score -= 80;

	//printf("\nwindow: %d, %d, %d, %d >> size: %d >> P2 count: %d >> NULL count: %d >> score: %d", window[0], window[1], window[2], window[3], size, count(window, size, PLAYER_2_TOKEN), count(window, size, EMPTY_SLOT), *score);
}

void getScore(struct hashmap* board, int* centres, int x, int y, int* finalScore) { // determines the best column to make a play in by giving each a score based on their current state
	int score = 0;

	//printf("\n");

	// centre score - moves made here give the AI more options
	for (int i = 0; i < 2; i++) {
		if (centres[i] != 0) { // prevents the check of a second centre column if there is only 1
			int* col = malloc(sizeof(int) * y);

			for (int j = 0; j < y; j++)
				col[j] = getToken(board, centres[i], j);

			// here we would ideally pass the ARRAY_LENGTH instead of y, but we cannot do this as the compiler won't know the array length after malloc,
			// it will only recognize a pointer, thus giving us the length of that instead
			score += count(col, y, PLAYER_2_TOKEN) * 6;
			
			free(col);
		}
	}
	
	// horizontal score
	for (int i = 0; i < y; i++) {
		int* row = malloc(sizeof(int) * x);
		
		//printf("\n");
		for (int j = 0; j < x; j++) {
			row[j] = getToken(board, j, i);
			//printf("%d, ", row[j]);
		}

		for (int j = 0; j < x - 3; j++) {
			int window[4] = { row[j], row[j + 1], row[j + 2], row[j + 3] };
			//printf("window %d = { %d, %d, %d, %d } >> P2 tokens: %d, ", j, row[j], row[j + 1], row[j + 2], row[j + 3], count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)));
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
		//printf("score: %d", score);
		
		free(row);
	}

	// vertical score
	for (int i = 0; i < x; i++) {
		int* col = malloc(sizeof(int) * y);

		for (int j = 0; j < y; j++)
			col[j] = getToken(board, i, j);

		for (int j = 0; j < y - 3; j++) {
			int window[4] = { col[j], col[j + 1], col[j + 2], col[j + 3] };
			//printf("window %d = { %d, %d, %d, %d } >> P2 tokens: %d, ", j, row[j], row[j + 1], row[j + 2], row[j + 3], count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)));
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}

		free(col);
	}
	
	// bottom-right to top-left diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { getToken(board, j, i), getToken(board, j + 1, i + 1), getToken(board, j + 2, i + 2), getToken(board, j + 3, i + 3) };
			//printf("window %d = { %d, %d, %d, %d } >> P2 tokens: %d, ", j, row[j], row[j + 1], row[j + 2], row[j + 3], count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)));
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
	}

	//bottom-left to top-right diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { getToken(board, j + 3, i), getToken(board, j + 2, i + 1), getToken(board, j + 1, i + 2), getToken(board, j, i + 3) };
			//printf("window %d = { %d, %d, %d, %d } >> P2 tokens: %d, ", j, row[j], row[j + 1], row[j + 2], row[j + 3], count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)));
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
	}

	*finalScore = score;
}

void pickBestMove(struct hashmap* board, int x, int y, int* column) {
	int score = 0, bestColumn = 0, bestScore = 0;

	int centres[2]; //you could maybe move this to the play method instead, to save processing time as these will be constants during the game
	// we need to determine if there is a literal center column, based on the board dimensions (x will be odd if there is)
	// if there isn't then we will evaluate the 2 centre columns (StackOverflow claims (x & 1) is faster at determining an odd number?)
	if (x % 2) {
		// is odd
		centres[0] = (int)round(x / 2.0f) - 1;
		centres[1] = 0; // we will use this to skip the double centre columns check
	}
	else {
		// is even
		centres[0] = (x / 2) - 1;
		centres[1] = centres[0] + 1;
	}
	/*printf("\n%d, %d", centres[0], centres[1]);
	delay(3);*/
	
	for (int i = 0; i < x; i++) {
		//if (!stackIsFull(hashGet(board, i))) { // this currently is preventing the AI's move, why?
		//maybe try if(!addMove(board, i, EMPTY_SLOT)) instead

		// this creates a temporary board which we will place a temporary move in for us to determine if it's a good move 
		struct hashmap* temp = createTable(x, y);
		for (int j = 0; j < x; j++) {
			for (int k = 0; k < y; k++) {
				int tok = getToken(board, j, k);
				if (tok != 0)
					addMove(temp, j, tok);
			}
		}

		bool full = addMove(temp, i, PLAYER_2_TOKEN);

		if (!full) {
			getScore(temp, centres, x, y, &score);
			if (score > bestScore) {
				bestScore = score;
				bestColumn = i;
			}
		}

		freeBoard(temp);
		//}
	}

	// + 1 here because we never want it to equal 0, otherwise it will close the game
	*column = bestColumn + 1;
}

// ideally we would calculate the array length here, but this isn't possible
// as the compiler doesn't know what the pointer is pointing to, so it cannot
// define the length of the array at compile time
int count(int* list, int n, int tok) {
	int c = 0;
	for (int i = 0; i < n; i++)
		if (list[i] == tok)
			c++;
	return c;
}

void freeBoard(struct hashmap* board) { // used to clear the board data from memory
	for (int i = 0; i < board->size; i++) {
		for (int j = 0; j < board->list[i]->stack->size; j++)
			free(board->list[i]->stack->list[j]);
		free(board->list[i]->stack);
		free(board->list[i]);
	}
	free(board->list);
	free(board);
}