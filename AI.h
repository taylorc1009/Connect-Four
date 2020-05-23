#include <math.h>
#include "hashmap.h"

#define PLAYER_1_TOKEN 1
#define PLAYER_2_TOKEN 2
#define EMPTY_SLOT 0

#define MINIMAX_DEPTH 3

#define ARRAY_LENGTH(x) ((int)sizeof(x) / sizeof((x)[0]))

void delay(int numOfSeconds) {
	int milliSeconds = 1000 * numOfSeconds;
	clock_t startTime = clock();
	while (clock() < startTime + milliSeconds);
}

struct Move {
	int column;
	int score;
	bool gameOver;
};

void freeBoard(struct hashmap* board);
struct Move* minimax(struct hashmap* board, int x, int y, int column, int* centres, int player, int depth);
void getScore(struct hashmap* board, int* centres, int x, int y, int* finalScore);

void AIMakeMove(struct hashmap* board, int* column, int* centres) {
	int x = getX(board), y = getY(board);
	struct Move* move = minimax(board, x, y, *column - 1, centres, PLAYER_2_TOKEN, MINIMAX_DEPTH);
	*column = move->column + 1;
	printf("\nfinal score & column = %d, %d", move->score, move->column + 1);
	free(move);
	delay(3);
}

struct hashmap* copyBoard(struct hashmap* board, int x, int y) {
	struct hashmap* copy = createTable(x, y);
	for (int j = 0; j < x; j++) {
		for (int k = 0; k < y; k++) {
			int tok = getToken(board, j, k);
			if (tok != 0)
				addMove(copy, j, tok);
		}
	}
	return copy;
}

bool isGameOver(struct hashmap* board, int row, int column) {
	printf("\ncheckWin P1 = %s, P2 = %s >> row: %d, column: %d\n", checkWin(row, column, board, PLAYER_1_TOKEN) ? "true" : "false", checkWin(row, column, board, PLAYER_2_TOKEN) ? "true" : "false", row, column);
	return checkWin(row, column, board, PLAYER_1_TOKEN) || checkWin(row, column, board, PLAYER_2_TOKEN) || isBoardFull(board, getX(board));
}

struct Move* minimax(struct hashmap* board, int x, int y, int column, int* centres, int player, int depth) {
	struct Move* move = (struct Move*)malloc(sizeof(struct Move));
	move->column = column;
	//printf("\n%d. (%d, %d), col: %d, tok: %d", depth, x, y, column, player);
	int row = hashGet(board, column)->top;
	move->gameOver = isGameOver(board, row, column);
	//printf(" >> gameOver? %s", move->gameOver ? "true" : "false");

	if (depth == 0 || move->gameOver) {
		if (move->gameOver) { //this still doesn't detect a win/loss sometimes, why?
			if (checkWin(row, column, board, PLAYER_2_TOKEN))
				move->score = 10000; // bot has won in this instance
			else if (checkWin(row, column, board, PLAYER_1_TOKEN))
				move->score = -10000; // player has won in this instance
			else
				move->score = 0; // board is full
			printf("\n>> gameOver, column = %d, row = %d, score = %d, tok = %d", column, row, move->score, player);
		}
		else
			getScore(board, centres, x, y, &(move)->score); // evaluate the state of the final instance
		printf(" >> score%d = %d", column, move->score);
		return move;
	}
	if (player == PLAYER_2_TOKEN) { // maximizing player
		move->score = -2147483648;
		for (int i = 0; i < x; i++) {
			if (!stackIsFull(hashGet(board, i))) {
				struct hashmap* temp = copyBoard(board, x, y);
				addMove(temp, i, PLAYER_2_TOKEN);

				for (int k = 0; k < y; k++) {
					printf("\n|");
					for (int j = 0; j < x; j++) {
						int p = getToken(temp, j, (y - 1) - k);
						printf("%d|", p);
					}
				}
				printf("d:%d", depth);

				struct Move* newMove = minimax(temp, x, y, i, centres, PLAYER_1_TOKEN, depth - 1);
				//printf("\nmove = { %d, %d }, newMove = { %d, %d }", move->score, move->column, newMove->score, newMove->column);
				if (newMove->score > move->score) {
					printf("\nnewMove = { %d, %d } > move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
					move->score = newMove->score;
					move->column = newMove->gameOver ? newMove->column : i;
					move->gameOver = newMove->gameOver;
					printf(" >> move->score changed = %d, column = %d", move->score, move->column);
				}
				freeBoard(temp);
				free(newMove);
				/*if (move->gameOver)
					return move;*/
			}
		}
		printf("\nstep up");
		return move;
	}
	else { // minimizing player
		move->score = 2147483647;
		for (int i = 0; i < x; i++) {
			if (!stackIsFull(hashGet(board, i))) {
				struct hashmap* temp = copyBoard(board, x, y);
				addMove(temp, i, PLAYER_1_TOKEN);

				for (int k = 0; k < y; k++) {
					printf("\n|");
					for (int j = 0; j < x; j++) {
						int p = getToken(temp, j, (y - 1) - k);
						printf("%d|", p);
					}
				}
				printf("d:%d", depth);

				struct Move* newMove = minimax(temp, x, y, i, centres, PLAYER_2_TOKEN, depth - 1);
				//printf("\nmove = { %d, %d }, newMove = { %d, %d }", move->score, move->column, newMove->score, newMove->column);
				if (newMove->score < move->score) {
					printf("\nnewMove = { %d, %d } < move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
					move->score = newMove->score;
					move->column = newMove->gameOver ? newMove->column : i;
					move->gameOver = newMove->gameOver;
					printf(" >> move->score changed = %d, column = %d", move->score, move->column);
				}
				freeBoard(temp);
				free(newMove);
				/*if (move->gameOver)
					return move;*/
			}
		}
		printf("\nstep up");
		return move;
	}
}

//struct Move* minimax(struct hashmap* board, int x, int y, int column, int* centres, int player, int depth) {
//	struct Move* move = (struct Move*)malloc(sizeof(struct Move));
//	move->column = 0;
//	//printf("\n%d. (%d, %d), col: %d, tok: %d", depth, x, y, column, player);
//	int row = hashGet(board, column)->top;/*, p;
//	if (player = PLAYER_2_TOKEN)
//		p = PLAYER_1_TOKEN;
//	else
//		p = PLAYER_2_TOKEN;*/
//	move->gameOver = isGameOver(board, row, column, player);
//	//printf(" >> gameOver? %s", move->gameOver ? "true" : "false");
//
//	if (depth == 0 || move->gameOver) {
//		if (move->gameOver) { //this still doesn't detect a win/loss sometimes, why?
//			if (checkWin(row, column, board, PLAYER_2_TOKEN))
//				move->score = 10000; // bot has won in this instance
//			else if (checkWin(row, column, board, PLAYER_1_TOKEN))
//				move->score = -10000; // player has won in this instance
//			else
//				move->score = 0; // board is full
//			printf("\n>> gameOver, column = %d, row = %d, score = %d, tok = %d", column, row, move->score, player);
//		}
//		else
//			getScore(board, centres, x, y, &(move)->score); // evaluate the state of the final instance
//		printf(" >> score%d = %d", column, move->score);
//		return move;
//	}
//	if (player == PLAYER_2_TOKEN) { // maximizing player
//		move->score = -2147483648;
//		for (int i = 0; i < x; i++) {
//			if (!stackIsFull(hashGet(board, i))) {
//				struct hashmap* temp = copyBoard(board, x, y);
//				addMove(temp, i, PLAYER_2_TOKEN);
//
//				for (int k = 0; k < y; k++) {
//					printf("\n|");
//					for (int j = 0; j < x; j++) {
//						int p = getToken(temp, j, (y - 1) - k);
//						printf("%d|", p);
//					}
//				}
//
//				struct Move* newMove = minimax(temp, x, y, i, centres, PLAYER_1_TOKEN, depth - 1);
//				//printf("\nmove = { %d, %d }, newMove = { %d, %d }", move->score, move->column, newMove->score, newMove->column);
//				if (newMove->score > move->score) {
//					printf("\nnewMove = { %d, %d } > move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
//					move->score = newMove->score;
//					move->column = i;
//					move->gameOver = newMove->gameOver;
//					printf(" >> move->score changed = %d, column = %d", move->score, move->column);
//				}
//				freeBoard(temp);
//				free(newMove);
//				if (move->gameOver)
//					return move;
//			}
//		}
//		printf("\n");
//		return move;
//	}
//	else { // minimizing player
//		move->score = 2147483647;
//		for (int i = 0; i < x; i++) {
//			if (!stackIsFull(hashGet(board, i))) {
//				struct hashmap* temp = copyBoard(board, x, y);
//				addMove(temp, i, PLAYER_1_TOKEN);
//
//				for (int k = 0; k < y; k++) {
//					printf("\n|");
//					for (int j = 0; j < x; j++) {
//						int p = getToken(temp, j, (y - 1) - k);
//						printf("%d|", p);
//					}
//				}
//
//				struct Move* newMove = minimax(temp, x, y, i, centres, PLAYER_2_TOKEN, depth - 1);
//				//printf("\nmove = { %d, %d }, newMove = { %d, %d }", move->score, move->column, newMove->score, newMove->column);
//				if (newMove->score < move->score) {
//					printf("\nnewMove = { %d, %d } < move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
//					move->score = newMove->score;
//					move->column = i;
//					move->gameOver = newMove->gameOver;
//					printf(" >> move->score changed = %d, column = %d", move->score, move->column);
//				}
//				freeBoard(temp);
//				free(newMove);
//				if (move->gameOver)
//					return move;
//			}
//		}
//		printf("\n");
//		return move;
//	}
//}

void evaluateWindow(int* window, int size, int* score) {
	if (count(window, size, PLAYER_2_TOKEN) == 2 && count(window, size, EMPTY_SLOT) == 2)
		*score += 2;
	else if (count(window, size, PLAYER_2_TOKEN) == 3 && count(window, size, EMPTY_SLOT) == 1)
		*score += 5;
	else if (count(window, size, PLAYER_2_TOKEN) == 4)
		*score += 100; // if minimax is being used, this is useless, but if we set the minimax depth to 0 this will have to be used (might use this for a difficulty modifier)
	else if (count(window, size, PLAYER_1_TOKEN) == 3 && count(window, size, EMPTY_SLOT) == 1)
		*score -= 4;

	//printf("\nwindow: %d, %d, %d, %d >> size: %d >> P2 count: %d >> NULL count: %d >> score: %d", window[0], window[1], window[2], window[3], size, count(window, size, PLAYER_2_TOKEN), count(window, size, EMPTY_SLOT), *score);
}

void getScore(struct hashmap* board, int* centres, int x, int y, int* finalScore) { // determines the best column to make a play in by giving each a score based on their current state
	int score = 0;

	// centre score - moves made here give the AI more options
	for (int i = 0; i < 2; i++) {
		if (centres[i] != 0) { // prevents the check of a second centre column if there is only 1
			int* col = malloc(sizeof(int) * y);

			for (int j = 0; j < y; j++)
				col[j] = getToken(board, centres[i], j);

			// here we would ideally pass the ARRAY_LENGTH instead of y, but we cannot do this as the compiler won't know the array length after malloc,
			// it will only recognize a pointer, thus giving us the length of that instead
			score += count(col, y, PLAYER_2_TOKEN) * 2;
			
			free(col);
		}
	}
	
	// horizontal score
	for (int i = 0; i < y; i++) {
		int* row = malloc(sizeof(int) * x);
		
		for (int j = 0; j < x; j++)
			row[j] = getToken(board, j, i);

		for (int j = 0; j < x - 3; j++) {
			int window[4] = { row[j], row[j + 1], row[j + 2], row[j + 3] };
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}		
		free(row);
	}

	// vertical score
	for (int i = 0; i < x; i++) {
		int* col = malloc(sizeof(int) * y);

		for (int j = 0; j < y; j++)
			col[j] = getToken(board, i, j);

		for (int j = 0; j < y - 3; j++) {
			int window[4] = { col[j], col[j + 1], col[j + 2], col[j + 3] };
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
		free(col);
	}
	
	// bottom-right to top-left diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { getToken(board, j, i), getToken(board, j + 1, i + 1), getToken(board, j + 2, i + 2), getToken(board, j + 3, i + 3) };
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
	}

	//bottom-left to top-right diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { getToken(board, j + 3, i), getToken(board, j + 2, i + 1), getToken(board, j + 1, i + 2), getToken(board, j, i + 3) };
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
	}

	*finalScore = score;
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