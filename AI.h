#include <math.h>
#include "hashmap.h"

#define PLAYER_1_TOKEN 1
#define PLAYER_2_TOKEN 2
#define EMPTY_SLOT 0

#define ARRAY_LENGTH(x) ((int)sizeof(x) / sizeof((x)[0]))

void pickBestMove(struct hashmap* board, int x, int y, int* column);
void freeBoard(struct hashmap* board);

void delay(int numOfSeconds) {
	int milliSeconds = 1000 * numOfSeconds;
	clock_t startTime = clock();
	while (clock() < startTime + milliSeconds);
}

void AIMakeMove(struct hashmap* board, int* column) {
	int x = getX(board), y = getY(board);
	pickBestMove(board, x, y, column);
	//printf("\npicked column: %d", *column);
	//delay(30);
}

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

void getScore(struct hashmap* board, int x, int y, int* finalScore) { // determines the best column to make a play in by giving each a score based on their current state
	int score = 0;

	//printf("\n");

	// centre score - gives the AI more moves
	// we need to determine if there is a literal center column, based on the board dimensions (x will be odd if there is)
	// if there isn't then we will evaluate the 2 centre columns (StackOverflow claims (x & 1) is faster at determining this?)
	if (x % 2) { //there's no point working this out here as we will need to do it for every column here
		// is odd
		int centre = (int)round(x / 2.0f);

	}
	else {
		// is even
		int centres[2] = { x / 2, centres[0] + 1 };

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
			/*if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 2 && count(window, EMPTY_SLOT, ARRAY_LENGTH(window)) == 2)
				score += 10;
			else if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 3 && count(window, EMPTY_SLOT, ARRAY_LENGTH(window)) == 1)
				score += 100;
			else if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 4)
				score += 1000;*/
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
			/*if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 2 && count(window, EMPTY_SLOT, ARRAY_LENGTH(window)) == 2)
				score += 10;
			else if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 3 && count(window, EMPTY_SLOT, ARRAY_LENGTH(window)) == 1)
				score += 100;
			else if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 4)
				score += 1000;*/
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}

		free(col);
	}
	
	// bottom-right to top-left diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { getToken(board, j, i), getToken(board, j + 1, i + 1), getToken(board, j + 2, i + 2), getToken(board, j + 3, i + 3) };
			//printf("window %d = { %d, %d, %d, %d } >> P2 tokens: %d, ", j, row[j], row[j + 1], row[j + 2], row[j + 3], count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)));
			/*if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 2 && count(window, EMPTY_SLOT, ARRAY_LENGTH(window)) == 2)
				score += 10;
			else if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 3 && count(window, EMPTY_SLOT, ARRAY_LENGTH(window)) == 1)
				score += 100;
			else if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 4)
				score += 1000;*/
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
	}

	//bottom-left to top-right diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { getToken(board, j + 3, i), getToken(board, j + 2, i + 1), getToken(board, j + 1, i + 2), getToken(board, j, i + 3) };
			//printf("window %d = { %d, %d, %d, %d } >> P2 tokens: %d, ", j, row[j], row[j + 1], row[j + 2], row[j + 3], count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)));
			/*if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 2 && count(window, EMPTY_SLOT, ARRAY_LENGTH(window)) == 2)
				score += 10;
			else if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 3 && count(window, EMPTY_SLOT, ARRAY_LENGTH(window)) == 1)
				score += 100;
			else if (count(window, PLAYER_2_TOKEN, ARRAY_LENGTH(window)) == 4)
				score += 1000;*/
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
	}

	*finalScore = score;
}

void pickBestMove(struct hashmap* board, int x, int y, int* column) {
	int score = 0, bestColumn = 0, bestScore = 0;
	
	for (int i = 0; i < x; i++) {
		if (!stackIsFull(hashGet(board, i))) { // this currently is preventing the AI's move, why?
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
				getScore(temp, x, y, &score);
				if (score > bestScore) {
					bestScore = score;
					bestColumn = i;
				}
			}

			freeBoard(temp);
		}
	}

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