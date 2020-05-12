#include "hashmap.h"

#define PLAYER_1_TOKEN 1
#define PLAYER_2_TOKEN 2

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
	printf("picked column: %d", *column);
	delay(20);
}

void getScore(struct hashmap* board, int x, int y, int* finalScore) { // determines the best column to make a play in by giving each a score based on their current state
	int score = 0;
	int* row = malloc(sizeof(int) * x);
	
	// horizontal check
	for (int i = 0; i < x; i++) {
		
		printf("\n");
		for (int j = 0; j < x; j++) {
			row[j] = getToken(board, j, i);
			printf("%d:%d, ", j, row[j]);
		}

		for (int j = 0; j < x - 3; j++) {
			int window[4] = { row[j], row[j + 1], row[j + 2], row[j + 3] };
			if (count(window, PLAYER_2_TOKEN) == 1 && count(window, NULL) == 3)
				score += 5;
			else if (count(window, PLAYER_2_TOKEN) == 2 && count(window, NULL) == 2)
				score += 10;
			else if (count(window, PLAYER_2_TOKEN) == 3 && count(window, NULL) == 1)
				score += 100;
		}
		
	}
	printf("\n");
	free(row);
	*finalScore = score;
}

void pickBestMove(struct hashmap* board, int x, int y, int* column) {
	int score = 0, bestColumn = 0, bestScore = 0;
	
	for (int i = 0; i < x; i++) {
		if (!stackIsFull(hashGet(board, i))) { // this currently is preventing the AI's move, why?
			// this creates a temporary board which we will place a temporary move in for us to determine if it's a good move 
			struct hashmap* temp = createTable(x, y);
			for (int j = 0; j < y; j++) {
				for (int k = 0; k < x; k++) {
					int tok = getToken(board, j, k);
					if (tok != 0)
						addMove(temp, j, tok);
				}
			}

			addMove(temp, i, PLAYER_2_TOKEN);

			getScore(temp, x, y, &score);
			if (score > bestScore) {
				bestScore = score;
				bestColumn = i;
			}

			freeBoard(temp);
		}
	}

	*column = bestColumn + 1;
}

int count(int* list, int tok) {
	int c = 0, n = arrayLength(list);
	for (int i = 0; i < n; i++)
		if (list[i] == tok)
			c++;
	return c;
}

int arrayLength(int* list) {
	return (int)sizeof(list) / sizeof(list[0]);
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

//bool isPlayableThreeInARow(int* window, int startX, int startY) {
//	int tCount = 0, eCount = 0;
//	for (int i = 0; i < 4; i++) {
//		if ((window[i] == PLAYER_2_TOKEN || window[i] == NULL) && tCount < 3 && eCount < 2)
//			count++;
//		else if (window[i] == PLAYER_1_TOKEN)
//			return false;
//	}
//}